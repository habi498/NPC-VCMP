# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/npcclient/dependencies/readline-8.2"
  "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/dependencies/libreadline-out"
  "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/dependencies/libreadline"
  "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/libreadline-prefix/tmp"
  "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/libreadline-prefix/src/libreadline-stamp"
  "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/libreadline-prefix/src"
  "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/libreadline-prefix/src/libreadline-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/libreadline-prefix/src/libreadline-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/libreadline-prefix/src/libreadline-stamp${cfgdir}") # cfgdir has leading slash
endif()
