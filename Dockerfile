# 第一阶段：基础开发环境
FROM ubuntu:24.04 AS base
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    ninja-build \
    python3 \
    bison \
    # libxcrypt 构建依赖
    libmount-dev \
    libblkid-dev \
    libcrypt-dev \
    libsystemd-dev \
    libssl-dev \
    libpcre2-dev \
    libxi-dev \
    libxtst-dev \
    meson \
    python3-jinja2 

RUN apt install -y autoconf automake libtool pkg-config

# 清理缓存
RUN rm -rf /var/lib/apt/lists/*

# 第二阶段：vcpkg 安装和依赖缓存
FROM base AS vcpkg
ENV VCPKG_ROOT=/opt/vcpkg
ENV VCPKG_DEFAULT_TRIPLET=x64-linux
# 启用二进制缓存
ENV VCPKG_BINARY_SOURCES="clear;default,readwrite"

# 安装 vcpkg
RUN git clone https://github.com/Microsoft/vcpkg.git ${VCPKG_ROOT}
RUN ${VCPKG_ROOT}/bootstrap-vcpkg.sh -disableMetrics
RUN ${VCPKG_ROOT}/vcpkg integrate install

# 仅复制 vcpkg.json 以利用缓存
COPY backend/vcpkg.json /backend/vcpkg.json
RUN cd /backend && ${VCPKG_ROOT}/vcpkg install \
    --x-manifest-root=. \
    --x-install-root=${VCPKG_ROOT}/installed \
    --feature-flags=binarycaching,manifests \
    --clean-after-build

# 第三阶段：构建项目
FROM vcpkg AS builder
WORKDIR /app

# 复制 CMake 文件
COPY CMakeLists.txt .
COPY backend ./backend
COPY frontend ./frontend

# 设置环境变量
ENV HOME=/app

# 配置 CMake，环境变量 HOME 为 /app
RUN cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
    -DVCPKG_TARGET_TRIPLET=${VCPKG_DEFAULT_TRIPLET} \
    -DBUILD_BACKEND=ON \
    -DBUILD_FRONTEND=OFF \
    -GNinja

# 构建项目
RUN cmake --build build --target backend_lib backend

# 最终阶段：运行环境
FROM ubuntu:24.04 AS runtime
ENV DEBIAN_FRONTEND=noninteractive

# 安装运行时依赖
RUN apt-get update && apt-get install -y \
    libcurl4 \
    libssl3 \
    && rm -rf /var/lib/apt/lists/*

# 创建非 root 用户
RUN useradd -m -s /bin/bash app
USER app

WORKDIR /app

ENV HOME=/app

# 从构建阶段复制必要的文件
COPY --from=builder /app/build/backend/backend .
COPY --from=builder /app/backend/KnowledgeBase ./KnowledgeBase
COPY --from=builder /opt/vcpkg/installed/x64-linux/lib/*.so* /usr/local/lib/

# 更新动态链接器缓存
USER root
RUN ldconfig
USER app

# 暴露端口
EXPOSE 8080

# 启动命令
CMD ["./backend"]
