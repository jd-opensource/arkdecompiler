FROM ubuntu:20.04

# 设置环境变量
ENV DEBIAN_FRONTEND=noninteractive \
    LANG=C.UTF-8 \
    ROOT_PATH=/root \
    DIRECTORY_NAME=harmonyos \
    GIT_REPO=https://github.com/jd-opensource/arkdecompiler.git \
    PATH=/root/bin:/root/harmonyos/prebuilts/build-tools/linux-x86/bin:$PATH

# 安装依赖
RUN apt-get update && apt-get install -y \
    python3 python3-pip ruby git-lfs gcc-multilib g++-multilib \
    zlib1g-dev libc++1 curl nodejs \
    && rm -rf /var/lib/apt/lists/*
RUN ln -s /usr/bin/python3 /usr/bin/python
# 配置 git 信息
RUN git config --global user.email "you@example.com" \
    && git config --global user.name "Your Name"

# 安装 repo 工具
RUN mkdir -p /root/bin \
    && curl -L https://gitee.com/oschina/repo/raw/fork_flow/repo-py3 -o /root/bin/repo \
    && chmod a+x /root/bin/repo \
    && pip3 install -i https://pypi.tuna.tsinghua.edu.cn/simple requests

# 创建源码目录并进入
WORKDIR /root/${DIRECTORY_NAME}

# 初始化并同步 OpenHarmony 源码
# 分支使用OpenHarmony-5.1.0-Release master分支有部分依赖的头文件被删除
RUN repo init -u https://gitee.com/ark-standalone-build/manifest.git -b OpenHarmony-5.1.0-Release \
    && repo sync -c -j8 \
    && repo forall -c 'git lfs pull' \
    && ./prebuilts_download.sh

# 克隆 arkdecompiler
RUN if [ ! -d arkdecompiler ]; then \
        git clone ${GIT_REPO}; \
    else \
        cd arkdecompiler && git pull; \
    fi

# 打 patch 添加xabc进入构建目标,修改BUILD.gn
RUN cp ./arkdecompiler/patches/arkcompiler_runtime_core_a94c360.patch ./arkcompiler/runtime_core/ \
    && cp ./arkdecompiler/patches/third_party_protobuf_0e4c27.patch ./third_party/protobuf/ \
    && cp ./arkdecompiler/patches/arkcompiler_ets_frontend_09e1955c6.patch ./arkcompiler/ets_frontend/ \
    && cd /root/harmonyos/arkcompiler/runtime_core \
    && git apply arkcompiler_runtime_core_a94c360.patch \
    && cd /root/harmonyos/third_party/protobuf \
    && git apply third_party_protobuf_0e4c27.patch \
    && cd /root/harmonyos/arkcompiler/ets_frontend \
    && git apply arkcompiler_ets_frontend_09e1955c6.patch

# 修改 run_command.py默认使用python3
RUN sed -i '1s|.*|#!/usr/bin/env python3|' /root/harmonyos/third_party/openssl/run_command.py

CMD ["/bin/bash"]
