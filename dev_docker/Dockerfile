FROM ubuntu:20.04

RUN sed -i s@/archive.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list
RUN sed -i s@/security.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list
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

# 创建用户 deploy，密码为 deploy
RUN useradd -m -s /bin/bash deploy && echo "deploy:deploy" | chpasswd && adduser deploy sudo

# 配置 SSH
RUN mkdir /var/run/sshd

# 允许 deploy 用户通过 SSH 登录
RUN sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
RUN sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config

# 创建 .ssh 目录并设置权限
# RUN mkdir -p /root/.ssh && chmod 700 /root/.ssh

# 将 id_rsa.pub 复制到容器内并添加到 authorized_keys 中
# COPY id_rsa.pub /root/.ssh/authorized_keys

# 设置权限
# RUN chmod 600 /root/.ssh/authorized_keys

# 暴露 SSH 端口
EXPOSE 22

# 设置默认工作目录
WORKDIR /home/deploy/workspace

# 启动 SSH 服务
CMD ["/usr/sbin/sshd", "-D"]
