#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "easy_profiler" for configuration "Debug"
set_property(TARGET easy_profiler APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(easy_profiler PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/libeasy_profiler.dll.a"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/libeasy_profiler.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS easy_profiler )
list(APPEND _IMPORT_CHECK_FILES_FOR_easy_profiler "${_IMPORT_PREFIX}/lib/libeasy_profiler.dll.a" "${_IMPORT_PREFIX}/bin/libeasy_profiler.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
