cmake_minimum_required (VERSION 3.8)
# Add source to this project's executable.
add_executable (recupdate
"main.cpp" )
include_directories( ${CMAKE_SOURCE_DIR} )
set(CMAKE_BUILD_TYPE Release)
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()