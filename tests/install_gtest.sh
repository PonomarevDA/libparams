#!/bin/bash
sudo apt-get update
sudo apt-get install -y make gcc-arm-none-eabi

sudo apt-get install libgtest-dev cmake
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp ./lib/libgtest*.a /usr/lib
