# - Config file for the FooBar package
# It defines the following variables
#  GORGON_INCLUDE_DIRS - include directories for FooBar
#  GORGON_LIBRARIES    - libraries to link against
#  FOOBAR_EXECUTABLE   - the bar executable
 
include("${CMAKE_CURRENT_LIST_DIR}/GorgonTargets.cmake")

# Compute paths
get_filename_component(GORGON_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
set(GORGON_INCLUDE_DIRS "/Data/programs/Tools/Educatool/GGE4/Source")
 
# Our library dependencies (contains definitions for IMPORTED targets)
#if(NOT TARGET foo AND NOT FooBar_BINARY_DIR)
#  include("${FOOBAR_CMAKE_DIR}/FooBarTargets.cmake")
#endif()
 
# These are IMPORTED targets created by GorgonTargets.cmake
set(GORGON_LIBRARIES Gorgon)
set(GORGON_EXECUTABLE )
