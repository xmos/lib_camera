#!/bin/bash

# clean everything
sudo rm -r build/


# build again
echo "Building everything up"
sleep 3 
cmake -B build -DCMAKE_TOOLCHAIN_FILE=xmos_cmake_toolchain/xs3a.cmake