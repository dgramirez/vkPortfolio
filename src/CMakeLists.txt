#Grab all the header files
file(GLOB_RECURSE FILES_H *.h *.hpp)

#Grab the correct compilable code
if (APPLE)
	file(GLOB_RECURSE FILES_SRC *.mm)
else(APPLE)
	file(GLOB_RECURSE FILES_SRC *.cpp *.c)
endif(APPLE)

#Create the executable
add_executable(vkPortfolio ${FILES_SRC} ${FILES_H})
