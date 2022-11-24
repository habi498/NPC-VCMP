cmake_minimum_required (VERSION 3.8)
# Add source to this project's executable.
add_executable (recupdate
"main.cpp" )
include_directories( ${CMAKE_SOURCE_DIR} )
set(CMAKE_BUILD_TYPE Release)