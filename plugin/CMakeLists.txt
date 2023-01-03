#CMake for server plugin npc04relxx
cmake_minimum_required (VERSION 3.8)
add_library(npc04 MODULE
"SQMain.cpp"
"SQFuncs.cpp"
"ConsoleUtils.cpp"
"PlayerRecording.cpp"
"SQConsts.cpp"
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(OUTPUT_BITNESS 64)
else()
	set(OUTPUT_BITNESS 32)
endif()

if(FORCE_32)
	set_target_properties(npc04 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	set(OUTPUT_BITNESS 32)
endif()
set_target_properties(npc04 PROPERTIES PREFIX "")
if(WIN32)
	if(OUTPUT_BITNESS EQUAL 64)
		set_target_properties(npc04 PROPERTIES OUTPUT_NAME "npc04rel64")
		message("WIN32 x64")
	else()
		set_target_properties(npc04 PROPERTIES OUTPUT_NAME "npc04rel32")
		message("WIN32 x32")
	endif()
else(WIN32)
	if(OUTPUT_BITNESS EQUAL 64)
		set_target_properties(npc04 PROPERTIES OUTPUT_NAME "npc04rel64")
		message("LINUX x64")
	else()
		set_target_properties(npc04 PROPERTIES OUTPUT_NAME "npc04rel32")
		message("LINUX x32")
	endif()
endif(WIN32)
set(CMAKE_BUILD_TYPE Release)
if(NOT WIN32)
add_definitions(-D__CUDACC__)
else()
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
