find_path(FLAC_INCLUDE_DIRS FLAC/stream_decoder.h)
find_path(FLAC_INCLUDE_DIRS flac/stream_decoder.h)

find_library(FLAC_LIBRARIES NAMES FLAC)

mark_as_advanced(FLAC_INCLUDE_DIRS FLAC_LIBRARIES)
