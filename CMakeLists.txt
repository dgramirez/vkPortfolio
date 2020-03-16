#Set Minimum Version
cmake_minimum_required(VERSION 3.16)

#Tell the user that Vulkan Demo is Generating
message("Derrick Ramirez Vulkan Portfolio")

#Win32 Setup
if (WIN32)
	#Specify to user it is a Win32 OS
	message("This is a Win32 OS")
	set(PLATFORM "Win32")

	#Ensure its x64 Machine
	set(CMAKE_EXE_LINKER_FLAGS "/machine:x64")
	if(${CMAKE_EXE_LINKER_FLAGS} MATCHES "/machine:x64")
		message("This is a x64 machine.")
		project(vkPortfolio)
		set(Architecture amd64)
		set(PLATFORM Win32)
	elseif(${CMAKE_EXE_LINKER_FLAGS} MATCHES "/machine:X86")
		message(FATAL_ERROR "Only x64 is supported.")
	endif()

	#Set Configuration Types for Debug and Release
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

	#Setup Output and Executable Directories
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif(WIN32)

#Linux Setup
if (UNIX AND NOT APPLE)
	#Specify to user it is a Linux OS
	message("This is Linux OS")
	set(PLATFORM "Unix")

	#Look for amd64 Debug/Release
	if(EXISTS ${CMAKE_BINARY_DIR}/debug OR EXISTS ${CMAKE_BINARY_DIR}/release)
		#Specify to user it is a x64 
		message("This is a x64 machine.")

		#Debug Setup
		if(EXISTS ${CMAKE_BINARY_DIR}/debug)
			message("Creating Debug Project")
			Project(vkPortfolio)
			SET(BUILD_TYPE Debug)
			message("Debug Configuration")
			SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64 -g -mmmx -msse  -msse2 -mfpmath=sse")
			SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -g -pthread -mmmx -msse  -msse2 -mfpmath=sse") # test: -std=c++1y -fms-extensions
			SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m64 -g -mmmx -msse  -msse2 -mfpmath=sse")
			add_definitions(-D_DEBUG)
		#Release Setup
		elseif(EXISTS ${CMAKE_BINARY_DIR}/release)
			Project(vkPortfolio)
			SET(BUILD_TYPE Release)
			message("Release Configuration")
			SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64 -mmmx -msse  -msse2 -mfpmath=sse")
			SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -pthread -mmmx -msse  -msse2 -mfpmath=sse ") # test: -std=c++1y -fms-extensions
			SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m64 -mmmx -msse  -msse2 -mfpmath=sse")
		endif()
	endif()

	#Turn on CodeLite use targets (For Executable)
	set(CMAKE_CODELITE_USE_TARGETS ON)
	
	#Set Location for Binary
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Bin/")
	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif(UNIX AND NOT APPLE)

if (APPLE)
	#Specify to user its MacOS
	message("This is Apple OS")
	set(PLATFORM "Apple")

	#Project Setup
	Project(vkPortfolio)
	Set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
	Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fmodules -fcxx-modules")
	
	#Set Location for Binary
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Bin/")
	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif(APPLE)


#Add Unicode Definitions
ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

add_subdirectory(src)
