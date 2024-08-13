#!/bin/bash

if [ "$1" == "debug" ]; then
    DEBUG_FLAG="DEBUG=1"
else
    DEBUG_FLAG=""
fi

make clean

make $DEBUG_FLAG

if [ $? -eq 0 ]; then
    echo "Build succeeded."
else
    echo "Build failed."
    exit 1
fi
