#!/bin/bash

echo ${PWD}
BASEDIR="$( cd "$( dirname "$0" )" && pwd )"

echo "Script location: ${BASEDIR}"
cd "${BASEDIR}"

cd ../
mkdir build
cd build

cmake ./../ -DCMAKE_OSX_ARCHITECTURES="x86_64" -G "Xcode"
