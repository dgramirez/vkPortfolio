#!/bin/bash

#Linux Setup
echo "Linux Setup. . ."
cd ../

#Make Build Directory
echo "Making Directories. . ."
mkdir build
mkdir build/debug
mkdir build/release
cd build

#Create Debug Workspace
cd debug
touch debug
cmake ../../ -G "CodeLite - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
rm debug
cd ../

#Create Release Workspace
cd release
touch release
cmake ../../ -G "CodeLite - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
rm release
cd ../

#Finished
echo "***Linux Setup Finish***"
