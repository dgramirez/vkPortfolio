::Echo off
@echo off

::Back one Directory
cd ../

::Check Existance, If so then delete it.
if exist "build" rd build /S /Q

::Create the build & VS Folder
mkdir build
mkdir "./build/VisualStudio"
cd build/VisualStudio

::Run CMake
cmake -A x64 "../../"

::Go back two directories and pause
cd ../../
pause
