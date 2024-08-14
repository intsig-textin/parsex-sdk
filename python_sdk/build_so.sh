#!/bin/bash

CURRENT_DIR=$(pwd)

# 进入指定的Makefile目录
cd ../linux_sdk/pybind || { echo "Directory not found"; exit 1; }

# 获取当前 Python 版本
PYTHON_VERSION=$(python3 -c "import sys; version=sys.version_info[:2]; print(f'{version[0]}.{version[1]}')")
PYTHON_DEV_PKG="python${PYTHON_VERSION}-dev"

# 检查是否安装了 pybind11
echo "Checking if pybind11 is installed..."
if ! python3 -c "import pybind11" 2>/dev/null; then
    echo "pybind11 not found. Installing..."
    python3 -m pip install pybind11 || { echo "Failed to install pybind11"; cd "$CURRENT_DIR"; exit 1; }
    sleep 1  # 等待安装完成
fi

# 检查操作系统类型
OS_TYPE=$(uname)

install_python_dev() {
    if [ "$OS_TYPE" == "Linux" ]; then
        # 检查 Linux 发行版（例如 Debian/Ubuntu 或 RedHat/CentOS）
        if [ -f /etc/debian_version ]; then
            if ! dpkg -s "$PYTHON_DEV_PKG" >/dev/null 2>&1 && ! [ -f "/usr/include/python${PYTHON_VERSION}m/Python.h" ]; then
                echo "${PYTHON_DEV_PKG} not found. Installing..."
                add-apt-repository ppa:deadsnakes/ppa -y
                apt-get update
                apt-get install -y "$PYTHON_DEV_PKG" || { echo "Failed to install ${PYTHON_DEV_PKG}"; cd "$CURRENT_DIR"; exit 1; }
            fi
        elif [ -f /etc/redhat-release ]; then
            # RedHat/CentOS
            PYTHON_DEV_PKG="python${PYTHON_VERSION}m-devel"
            if ! rpm -q "$PYTHON_DEV_PKG" >/dev/null 2>&1; then
                echo "${PYTHON_DEV_PKG} not found. Installing..."
                yum install -y "$PYTHON_DEV_PKG" || { echo "Failed to install ${PYTHON_DEV_PKG}"; cd "$CURRENT_DIR"; exit 1; }
            fi
        else
            echo "Unsupported Linux distribution."
            cd "$CURRENT_DIR"
            exit 1
        fi
    elif [ "$OS_TYPE" == "Darwin" ]; then
        # macOS
        PYTHON_DEV_PKG="python@${PYTHON_VERSION}"
        if ! brew list "$PYTHON_DEV_PKG" >/dev/null 2>&1; then
            echo "${PYTHON_DEV_PKG} not found. Installing..."
            brew install "$PYTHON_DEV_PKG" || { echo "Failed to install ${PYTHON_DEV_PKG}"; cd "$CURRENT_DIR"; exit 1; }
        fi
    else
        echo "Unsupported OS: $OS_TYPE"
        cd "$CURRENT_DIR"
        exit 1
    fi
}

# 检查并安装 python 开发库
echo "Checking if ${PYTHON_DEV_PKG} is installed..."
install_python_dev

# 设置Debug标志
if [ "$1" == "debug" ]; then
    DEBUG_FLAG="DEBUG=1"
else
    DEBUG_FLAG=""
fi

# 执行make clean和make命令
make clean

make $DEBUG_FLAG

# 检查编译是否成功
if [ $? -eq 0 ]; then
    echo "Build succeeded."
else
    echo "Build failed."
    # 编译失败回到原来的目录
    cd "$CURRENT_DIR"
    exit 1
fi

# 编译完成后回到之前的目录
cd "$CURRENT_DIR"
