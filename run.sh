#!/bin/bash

# create build directory if not exists
if [ ! -d "build" ]; then
    mkdir build
fi

# remove old files in build directory
rm -rf build/*

# move to build directory
cd build

# compile watchdog.cpp
g++ -o watchDog ../src/watchdog.cpp 

# execute watchDog binary
./watchDog