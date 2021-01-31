### Cmake Function ###
function(FindVulkan TARGET VKLIBNAME)
	#Find and Add Vulkan
	if (VULKAN_SDK)
		target_include_directories(${TARGET} PUBLIC ${VULKAN_SDK}/include/)
		target_link_directories(${TARGET} PUBLIC ${VULKAN_SDK}/lib/)
		target_link_libraries(${TARGET} PUBLIC ${VKLIBNAME})
	else()
		find_package(Vulkan REQUIRED)
		target_include_directories(${TARGET} PUBLIC ${Vulkan_INCLUDE_DIR})
		target_link_directories(${TARGET} PUBLIC ${Vulkan_LIBRARIES})
		target_link_libraries(${TARGET} PUBLIC ${VKLIBNAME})
	endif(VULKAN_SDK)
endfunction(FindVulkan TARGET VKLIBNAME)

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
	FindVulkan(vkPortfolio "vulkan-1.lib")
	target_include_directories(vkPortfolio PUBLIC "../dep/consolecolors/")
	target_include_directories(vkPortfolio PUBLIC "../dep/Gateware/")
	target_include_directories(vkPortfolio PUBLIC "../dep/imgui/")
	target_include_directories(vkPortfolio PUBLIC "../dep/sse_math/")
	target_include_directories(vkPortfolio PUBLIC "../dep/stbimage/")
	target_include_directories(vkPortfolio PUBLIC "../dep/vmaAllocator/")

	#Set the property for the linker language to be CXX
	set_property(TARGET vkPortfolio PROPERTY LINKER_LANGUAGE CXX)
endif(WIN32)

if (UNIX AND NOT APPLE)
	#Find and Add X11
	find_package(X11 REQUIRED)
	link_directories(${X11_INCLUDE_DIR})
	link_libraries(${X11_LIBRARIES})

	#Add all needed files from this directory and sub-directories
	file(GLOB_RECURSE FILES_H *.h)
	file(GLOB_RECURSE FILES_SRC *.cpp)
	file(GLOB CONSOLECOLOR ../dep/consolecolors/*.c)
	file(GLOB IMGUI ../dep/imgui/*.cpp)

	#Create the project as exe/binary
	add_executable(vkPortfolio ${FILES_SRC} ${FILES_H} ${CONSOLECOLOR} ${IMGUI})

	#Set the include & lib directories
	FindVulkan(vkPortfolio "vulkan.so")
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
	
	#Create the project as exe/binary
	add_executable(vkPortfolio ${FILES_SRC} ${FILES_H})

	#Add Vulkan
	FindVulkan(vkPortfolio "vulkan.dylib")
endif(APPLE)

#Set Precompiled Header
target_precompile_headers( vkPortfolio PUBLIC "./pch.h")
