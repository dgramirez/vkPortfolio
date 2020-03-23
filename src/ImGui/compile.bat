@echo off
set shdr=imgui
%VULKAN_SDK%\Bin\glslangValidator.exe -V %shdr%.vert
%VULKAN_SDK%\Bin\glslangValidator.exe -V %shdr%.frag

@echo off
move ./vert.spv ./%shdr%.vert.spv
move ./frag.spv ./%shdr%.frag.spv
PAUSE