#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Gorgon" for configuration "Debug"
set_property(TARGET Gorgon APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Gorgon PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C;CXX"
  IMPORTED_LOCATION_DEBUG "/usr/local/lib/libGorgon_d.a"
  )

list(APPEND _cmake_import_check_targets Gorgon )
list(APPEND _cmake_import_check_files_for_Gorgon "/usr/local/lib/libGorgon_d.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
