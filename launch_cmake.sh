#!/bin/bash

# clean everything
sudo rm -r build/*
sudo rm -r modules/core modules/i2c modules/mipi 
echo "Running west update"
sleep 2 
west update

# build again
echo "Building everything up"
sleep 3 
cmake -B build -DCMAKE_TOOLCHAIN_FILE=xmos_cmake_toolchain/xs3a.cmake