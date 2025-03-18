
# https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/subsystems/subsys-arkcompiler-guide.md

intall_deps(){
    sudo apt-get update && sudo apt-get install python ruby python3-pip git-lfs gcc-multilib g++-multilib zlib1g-dev libc++1 curl nodejs
}

intall_repo(){
    mkdir ~/bin/
    curl https://gitee.com/oschina/repo/raw/fork_flow/repo-py3 > ~/bin/repo
    chmod a+x ~/bin/repo
    export PATH=~/bin:$PATH
    pip3 install -i https://pypi.tuna.tsinghua.edu.cn/simple requests
}

download_source(){

    repo init -u https://gitee.com/ark-standalone-build/manifest.git -b master
    repo sync -c -j8
    repo forall -c 'git lfs pull'

    ./prebuilts_download.sh
}

intall_deps
intall_repo
download_source

#python3 ark.py x64.release



