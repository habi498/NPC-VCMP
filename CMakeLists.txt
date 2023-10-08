cmake_minimum_required (VERSION 3.15)

#For /MT (Multithreaded) - compiler is MSVC 
cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project ("npc-vcmp")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${PROJECT_SOURCE_DIR}/bin>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${PROJECT_SOURCE_DIR}/bin>)


# Include sub-projects.
if(WIN32)
	#For windows just build 32 bit binary
	if(CMAKE_SIZEOF_VOID_P EQUAL 4 OR FORCE_32)
		add_subdirectory ("npcclient")
	endif()
else()
	#For Linux build both 32 bit and 64 bit
	add_subdirectory("npcclient")

endif()
if(REL004)
message(STATUS "Skipping plugin for REL004, since rel004 servers can use npc rel 006 plugin")
else()
	add_subdirectory ("plugin")
	if((NOT WIN32) OR( WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 4))
		# make recupdate always 32 bit in windows
		add_subdirectory ("tools/recupdate")
		add_subdirectory ("npcclient-plugins/stdrpc")
		add_subdirectory("npcclient-plugins/z-finder")
		add_subdirectory("npcclient-plugins/actionlib")
		# npc client in windows always 32 bit, so it's plugin also 32 bit
		add_subdirectory ("npcclient-plugins/npc-plugin")
	endif()
endif()

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE PATH "" FORCE)

#Install to bin
if(TARGET npcclient)
install(TARGETS npcclient DESTINATION bin)
endif()

#the plugin
if(TARGET npc04) 
install(TARGETS npc04 DESTINATION bin)
endif()

#the plugins loaded by npcclient
# 1. actionlib
if(TARGET actionlib)
install(TARGETS actionlib DESTINATION ${PROJECT_SOURCE_DIR}/bin)
endif()
# 2. rpclib
if(TARGET rpclib)
install(TARGETS rpclib DESTINATION ${PROJECT_SOURCE_DIR}/bin)
endif()
# 3. z-finder
if(TARGET z-finder)
install(TARGETS z-finder DESTINATION ${PROJECT_SOURCE_DIR}/bin)
endif()