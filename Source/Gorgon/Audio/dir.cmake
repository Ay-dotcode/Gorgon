SET(Local
	Audio.cpp
)

IF(AUDIOLIB STREQUAL "PULSE")
	LIST(APPEND Local PulseAudio.cpp)
ENDIF()