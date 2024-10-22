#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "squirrel::squirrel_static" for configuration "Release"
set_property(TARGET squirrel::squirrel_static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(squirrel::squirrel_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libsquirrel_static.a"
  )

list(APPEND _cmake_import_check_targets squirrel::squirrel_static )
list(APPEND _cmake_import_check_files_for_squirrel::squirrel_static "${_IMPORT_PREFIX}/lib/libsquirrel_static.a" )

# Import target "squirrel::sqstdlib_static" for configuration "Release"
set_property(TARGET squirrel::sqstdlib_static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(squirrel::sqstdlib_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libsqstdlib_static.a"
  )

list(APPEND _cmake_import_check_targets squirrel::sqstdlib_static )
list(APPEND _cmake_import_check_files_for_squirrel::sqstdlib_static "${_IMPORT_PREFIX}/lib/libsqstdlib_static.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
