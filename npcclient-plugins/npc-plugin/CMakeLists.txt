
cmake_minimum_required (VERSION 3.15)
#For /MT (Multithreaded) - compiler is MSVC 
cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
project("npcmodule") 
# Add source to this project's executable.
add_library(npcmodule MODULE
"main.cpp" )
set_target_properties(npcmodule PROPERTIES PREFIX "")
if(MSVC)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
set(CMAKE_BUILD_TYPE Release)
if(FORCE_32)
	set_target_properties(npcmodule PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	if(WIN32)
		set_target_properties(npcmodule PROPERTIES OUTPUT_NAME "npcmodule")
	else()
		set_target_properties(npcmodule PROPERTIES OUTPUT_NAME "npcmodule32")
	endif()
elseif((CMAKE_SIZEOF_VOID_P EQUAL 8) AND (NOT WIN32))
	set_target_properties(npcmodule PROPERTIES OUTPUT_NAME "npcmodule64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8) 
	message(FATAL_ERROR "Module for npcclient on windows must be 32-bit")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	if(WIN32)
	set_target_properties(npcmodule PROPERTIES OUTPUT_NAME "npcmodule")
	else()
	set_target_properties(npcmodule PROPERTIES OUTPUT_NAME "npcmodule32")
	endif()
endif()



