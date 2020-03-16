@echo off

cd ../
if not exist "build" mkdir build
cd build

if not exist "VisualStudio" mkdir VisualStudio
cd VisualStudio

cmake -A x64 "../../"
cmake -A x64 "../../"

if errorlevel 1 (
    echo No appropriate VS compiler found. Error Level: %errorlevel%
)
cd ../../
pause
