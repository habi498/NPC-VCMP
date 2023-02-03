cmake_minimum_required (VERSION 3.8)
# Add source to this project's executable.
add_library(npcmodule MODULE
"main.cpp" )
target_include_directories(npcmodule PRIVATE "${PROJECT_SOURCE_DIR}/npc-plugin")
set_target_properties(npcmodule PROPERTIES PREFIX "")
set(CMAKE_BUILD_TYPE Release)

