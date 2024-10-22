# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/npcclient/dependencies/ncurses-6.4"
  "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/dependencies/libncurses-out"
  "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/dependencies/libncurses"
  "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/libncurses-prefix/tmp"
  "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/libncurses-prefix/src/libncurses-stamp"
  "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/libncurses-prefix/src"
  "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/libncurses-prefix/src/libncurses-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/libncurses-prefix/src/libncurses-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/libncurses-prefix/src/libncurses-stamp${cfgdir}") # cfgdir has leading slash
endif()
