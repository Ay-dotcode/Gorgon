CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

MACRO(DoSource)
	#IF(${wd} MATCHES ".+")
	#	MESSAGE("> Listing ${wd}")
	#ELSE()
	#	MESSAGE("> Listing Root")
	#ENDIF()
	
	STRING(REGEX REPLACE "/" "\\\\" srcgrp "${wd}")
	STRING(REGEX REPLACE "Source" "" srcgrp "${srcgrp}")
	STRING(REGEX REPLACE "^\\\\" "" srcgrp "${srcgrp}")
	
	IF(${wd} MATCHES ".+")
		SET(LocalFixed)
		SET(LocalShaders)
		FOREACH(L ${Local}) 
			IF(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/${wd}/${L}")
			ELSEIF(L MATCHES ".*\\.glsl")
				LIST(APPEND LocalShaders "${wd}/${L}")
			ELSE()
				LIST(APPEND LocalFixed "${wd}/${L}")
			ENDIF()
		ENDFOREACH()
	ELSE()
		SET(LocalFixed)
		SET(LocalShaders)
		FOREACH(L ${Local}) 
			IF(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/${wd}/${L}")
			ELSEIF(L MATCHES ".*\\.glsl")
				LIST(APPEND LocalShaders ${L})
			ELSE()
				LIST(APPEND LocalFixed ${L})
			ENDIF()
		ENDFOREACH()
	ENDIF()

	LIST(APPEND All ${LocalFixed} ${LocalShaders})
	LIST(LENGTH LocalFixed len)
	IF(len GREATER 7)
		SET(headergrpfiles)
		SET(srcgrpfiles)
		 
		FOREACH(S ${LocalFixed})
			IF(S MATCHES ".*\\.h")
				LIST(APPEND headergrpfiles ${S})
			ELSE()
				LIST(APPEND srcgrpfiles ${S})
			ENDIF()
		ENDFOREACH()
		
		SOURCE_GROUP("${srcgrp}" FILES ${headergrpfiles})
		SOURCE_GROUP("${srcgrp}\\Source" FILES ${srcgrpfiles})
	ELSE()
		SOURCE_GROUP("${srcgrp}" FILES ${LocalFixed})
	ENDIF()
	
	SOURCE_GROUP("${srcgrp}\\Shaders" FILES ${LocalShaders})
	
	FOREACH(S ${Local})
		IF(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/${wd}/${S}")
			IF("${wd}" MATCHES "^[^\\/]+")
				SET(wd "${wd}/${S}")
			ELSE()
				SET(wd "${S}")
			ENDIF()

			LIST(APPEND pwd "${wd}")
			
			INCLUDE("${wd}/dir.cmake")
			DoSource()

			LIST(REMOVE_AT pwd -1)
			LIST(GET pwd -1 wd)
		ENDIF()
	ENDFOREACH()
ENDMACRO()

MACRO(StartSource src)
	SET(wd)
	SET(pwd "/")
	SET(testid 0)
	SET(Local ${src})
	SET(ExcludeDoc)
	SET(deps)
	DoSource()
ENDMACRO()

list(APPEND deps ShaderEmbedder)

MACRO(EmbedShaders out for) #inputs
	set(listv)
	
	foreach(a ${ARGN})
		list(APPEND listv ${CMAKE_SOURCE_DIR}/${wd}/${a})
	endforeach()
	
	add_custom_command(
		OUTPUT ${CMAKE_SOURCE_DIR}/${wd}/${out}
		COMMAND "${CMAKE_SOURCE_DIR}/Tools/ShaderEmbedder/Bin/ShaderEmbedder" ${out} ${ARGN}
		DEPENDS ShaderEmbedder ${listv}
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/${wd}"
		COMMENT "Embedding shaders into ${out}"
	)
	
	list(APPEND Local ${ARGN})
	list(APPEND Local ${out})
	
	set(cur get_property(SOURCE ${CMAKE_SOURCE_DIR}/${wd}/${for} PROPERTY OBJECT_DEPENDS))
	set_property(SOURCE ${CMAKE_SOURCE_DIR}/${wd}/${for} APPEND PROPERTY OBJECT_DEPENDS "${cur};${CMAKE_SOURCE_DIR}/${wd}/${out}")
ENDMACRO()
