#!/bin/bash

MAKEFILE_DIR=$(dirname "$0")

cd "$MAKEFILE_DIR"

if [ "$1" == "debug" ]; then
    DEBUG_FLAG="DEBUG=1"
else
    DEBUG_FLAG=""
fi

mkdir -p build

make clean

make $DEBUG_FLAG

if [ $? -eq 0 ]; then
    echo "Build succeeded."
else
    echo "Build failed."
    exit 1
fi
