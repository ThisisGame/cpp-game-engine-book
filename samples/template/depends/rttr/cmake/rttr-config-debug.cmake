#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "RTTR::Core_Lib" for configuration "Debug"
set_property(TARGET RTTR::Core_Lib APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(RTTR::Core_Lib PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX;RC"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/librttr_core_d.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS RTTR::Core_Lib )
list(APPEND _IMPORT_CHECK_FILES_FOR_RTTR::Core_Lib "${_IMPORT_PREFIX}/lib/librttr_core_d.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
