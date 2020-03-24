#Windows Setup
if (WIN32)
	#Add all needed files from this directory and sub-directories
	file(GLOB_RECURSE FILES_H *.h)
	file(GLOB_RECURSE FILES_SRC *.cpp)
	file(GLOB CONSOLECOLOR ../dep/consolecolors/*.c)
	file(GLOB IMGUI ../dep/imgui/*.cpp)

	#Create the project as exe/binary
	add_executable(vkPortfolio ${FILES_SRC} ${FILES_H} ${CONSOLECOLOR} ${IMGUI})

	#Set the include & lib directories
	target_include_directories(vkPortfolio PUBLIC "../dep/consolecolors/")
	target_include_directories(vkPortfolio PUBLIC "../dep/Gateware/")
	target_include_directories(vkPortfolio PUBLIC "../dep/imgui/")
	target_include_directories(vkPortfolio PUBLIC "../dep/sse_math/")
	target_include_directories(vkPortfolio PUBLIC "../dep/stbimage/")
	target_include_directories(vkPortfolio PUBLIC "../dep/vmaAllocator/")
	target_include_directories(vkPortfolio PUBLIC $ENV{VULKAN_SDK}/Include/)
	target_link_directories(vkPortfolio PUBLIC $ENV{VULKAN_SDK}/Lib/)
	target_link_libraries(vkPortfolio PUBLIC "vulkan-1.lib")

	#Set the property for the linker language to be CXX
	set_property(TARGET vkPortfolio PROPERTY LINKER_LANGUAGE CXX)
endif(WIN32)

if (UNIX AND NOT APPLE)
	#Find and Add X11
	find_package(X11 REQUIRED)
	link_directories(${X11_INCLUDE_DIR})
	link_libraries(${X11_LIBRARIES})

	#Find and Add Vulkan
	find_package(Vulkan REQUIRED)
	link_directories(${Vulkan_INCLUDE_DIR})
	link_libraries(${Vulkan_LIBRARIES})

	#Add all needed files from this directory and sub-directories
	file(GLOB_RECURSE FILES_H *.h)
	file(GLOB_RECURSE FILES_SRC *.cpp)
	file(GLOB CONSOLECOLOR ../dep/consolecolors/*.c)
	file(GLOB IMGUI ../dep/imgui/*.cpp)

	#Create the project as exe/binary
	add_executable(vkPortfolio ${FILES_SRC} ${FILES_H} ${CONSOLECOLOR} ${IMGUI})

	#Set the include & lib directories
	target_include_directories(vkPortfolio PUBLIC "../dep/consolecolors/")
	target_include_directories(vkPortfolio PUBLIC "../dep/Gateware/")
	target_include_directories(vkPortfolio PUBLIC "../dep/imgui/")
	target_include_directories(vkPortfolio PUBLIC "../dep/sse_math/")
	target_include_directories(vkPortfolio PUBLIC "../dep/stbimage/")
	target_include_directories(vkPortfolio PUBLIC "../dep/vmaAllocator/")

endif(UNIX AND NOT APPLE)

if (APPLE)
	#Add all needed files from this directory and sub-directories
	file(GLOB_RECURSE FILES_H *.h)
	file(GLOB_RECURSE FILES_SRC *.mm)

	#Find Vulkan package
	find_package(Vulkan)
	if(Vulkan_FOUND)
	    message("Vulkan was found! attempting to link.")
	    link_directories(${Vulkan_INCLUDE_DIR})
	    link_libraries(${Vulkan_LIBRARIES})
	endif(Vulkan_FOUND)
	
	#Create the project as exe/binary
	add_executable(vkPortfolio ${FILES_SRC} ${FILES_H})
endif(APPLE)

#Set Precompiled Header
target_precompile_headers( vkPortfolio PUBLIC "./pch.h")
