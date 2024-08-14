#!/bin/bash

CURRENT_DIR=$(pwd)

# 进入指定的Makefile目录
cd ../linux_sdk/pybind || { echo "Directory not found"; exit 1; }

# 检查是否安装了 pybind11
echo "Checking if pybind11 is installed..."
if ! python3 -c "import pybind11" 2>/dev/null; then
    echo "pybind11 not found. Installing..."
    pip3 install pybind11 || { echo "Failed to install pybind11"; cd "$CURRENT_DIR"; exit 1; }
    sleep 1  # 等待安装完成
fi

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
