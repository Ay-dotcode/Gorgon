find_path(FLAC_INCLUDE_DIRS FLAC/stream_decoder.h)
find_path(FLAC_INCLUDE_DIRS flac/stream_decoder.h)

find_library(FLAC_LIBRARIES NAMES FLAC)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FLAC DEFAULT_MSG FLAC_LIBRARY FLAC_INCLUDE_DIR)

mark_as_advanced(FLAC_INCLUDE_DIRS FLAC_LIBRARIES)
