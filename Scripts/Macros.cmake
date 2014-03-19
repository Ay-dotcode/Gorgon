cmake_minimum_required(VERSION 2.8)

MACRO(DoSource)
	IF(${wd} MATCHES ".+")
		MESSAGE("> Building ${wd}")
	ELSE()
		MESSAGE("> Building Root")
	ENDIF()
	
	STRING(REGEX REPLACE "/" "\\\\" srcgrp "${wd}")
	STRING(REGEX REPLACE "Source" "" srcgrp "${srcgrp}")
	STRING(REGEX REPLACE "^\\\\" "" srcgrp "${srcgrp}")
	SOURCE_GROUP("${srcgrp}" FILES ${Local})

	LIST(APPEND All ${Local})

	FOREACH(S ${Sub})
		IF("${wd}" MATCHES "^[^\\/]+")
			SET(wd "${wd}/${S}")
		ELSE()
			SET(wd "${S}")
		ENDIF()

		LIST(APPEND pwd "${wd}")
		
		INCLUDE("${wd}/dir.cmake")

		LIST(REMOVE_AT pwd -1)
		LIST(GET pwd -1 wd)
	ENDFOREACH()
ENDMACRO()

MACRO(StartSource src)
	SET(wd)
	SET(pwd "/")
	SET(Local)
	SET(Sub ${src})
	DoSource()
ENDMACRO()
