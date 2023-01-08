cmake_minimum_required (VERSION 3.8)
# Add source to this project's executable.
add_library(mercedes MODULE
"main.cpp" )
target_include_directories(mercedes PRIVATE "${PROJECT_SOURCE_DIR}/npc-plugin")
set(CMAKE_BUILD_TYPE Release)

