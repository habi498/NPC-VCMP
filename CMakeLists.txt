# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.

cmake_minimum_required (VERSION 3.8)
project ("npc-vcmp")
# Include sub-projects.
if(WIN32)
	#For windows just build 32 bit binary
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		add_subdirectory ("npcclient")
	endif()
else()
	#For Linux build both 32 bit and 64 bit
	add_subdirectory("npcclient")
endif()
add_subdirectory ("plugin")
