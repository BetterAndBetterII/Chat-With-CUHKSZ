FROM ubuntu:24.04

RUN apt-get clean
RUN apt-get update

ARG DEBIAN_FRONTEND=noninteractive

# 安装必要的工具
RUN apt-get update
RUN apt-get install -y \
    sudo \
    openssh-server \
    cmake \
    make \
    g++ \
    build-essential

RUN apt-get install -y \
    git \
    vim \
    curl \
    wget \
    unzip \
    tar \
    zip \
    gcc gdb gdbserver \
    # 无需libboost可以去掉下一行
    libboost-dev \
    # net-tools 提供了ifconfig
    net-tools rsync

# 暴露 SSH 端口
EXPOSE 22

# 编译CMakeLists.txt，指定目标为backend
RUN cmake -Bbuild -Hbackend
RUN cmake --build build -j14

# 设置工作目录
WORKDIR /backend

# 启动SSH服务
CMD ["top"]
