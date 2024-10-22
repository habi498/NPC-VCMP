# Install script for directory: /mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/npcclient/dependencies/squirrel-master

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Development" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/npcclient/dependencies/squirrel-master/include/sqconfig.h"
    "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/npcclient/dependencies/squirrel-master/include/squirrel.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Development" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/npcclient/dependencies/squirrel-master/include/sqstdaux.h"
    "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/npcclient/dependencies/squirrel-master/include/sqstdblob.h"
    "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/npcclient/dependencies/squirrel-master/include/sqstdio.h"
    "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/npcclient/dependencies/squirrel-master/include/sqstdmath.h"
    "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/npcclient/dependencies/squirrel-master/include/sqstdstring.h"
    "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/npcclient/dependencies/squirrel-master/include/sqstdsystem.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Development" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/squirrel" TYPE FILE FILES
    "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/dependencies/squirrel-master/lib/cmake/squirrel/squirrel-config-version.cmake"
    "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/dependencies/squirrel-master/lib/cmake/squirrel/squirrel-config.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Development" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/squirrel/squirrel-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/squirrel/squirrel-targets.cmake"
         "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/dependencies/squirrel-master/CMakeFiles/Export/dafcadaba40601724742fc9ee142f030/squirrel-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/squirrel/squirrel-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/squirrel/squirrel-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/squirrel" TYPE FILE FILES "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/dependencies/squirrel-master/CMakeFiles/Export/dafcadaba40601724742fc9ee142f030/squirrel-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/squirrel" TYPE FILE FILES "/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/dependencies/squirrel-master/CMakeFiles/Export/dafcadaba40601724742fc9ee142f030/squirrel-targets-release.cmake")
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/dependencies/squirrel-master/squirrel/cmake_install.cmake")
  include("/mnt/hgfs/C/Users/hhabi/source/repos/habi498/NPC-VCMP/out-linux/npcclient/dependencies/squirrel-master/sqstdlib/cmake_install.cmake")

endif()

