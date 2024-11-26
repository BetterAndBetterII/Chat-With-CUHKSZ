FROM ubuntu:24.04

# 设置环境变量
ENV DEBIAN_FRONTEND=noninteractive
ENV VCPKG_ROOT=/opt/vcpkg
ENV VCPKG_DEFAULT_TRIPLET=x64-linux

RUN apt-get clean
RUN apt-get update

ARG DEBIAN_FRONTEND=noninteractive

# 安装基本工具和编译环境
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
    && rm -rf /var/lib/apt/lists/*

# 安装并配置 vcpkg
RUN git clone https://github.com/Microsoft/vcpkg.git ${VCPKG_ROOT} \
    && ${VCPKG_ROOT}/bootstrap-vcpkg.sh -disableMetrics \
    && ${VCPKG_ROOT}/vcpkg integrate install

# 复制 vcpkg.json 到容器中
COPY vcpkg.json /tmp/vcpkg.json

# 预安装依赖
RUN cd /tmp && ${VCPKG_ROOT}/vcpkg install --x-manifest-root=. --x-install-root=${VCPKG_ROOT}/installed

# 创建工作目录
WORKDIR /app

# 复制源代码
COPY . .

# 构建项目
RUN cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
    -DVCPKG_TARGET_TRIPLET=${VCPKG_DEFAULT_TRIPLET} \
    -GNinja \
    && cmake --build build --target backend_lib booking_test

# 设置工作目录为构建目标所在目录
WORKDIR /app/build/backend

# 暴露后端服务端口（如果需要）
EXPOSE 8080

# 启动后端服务
CMD ["./booking_test"]