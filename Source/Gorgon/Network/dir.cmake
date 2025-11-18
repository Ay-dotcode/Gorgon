SET(Local
	
)

OPTION(HTTP "Enable HTTP data and file transfer. Requires libcurl. [On Windows] Visual Studio static linking should be changed in libcurl project. Compiled libcurl libraries (static lib, debug and release) and include files should be placed in Source/External/curl. " OFF)

IF(${HTTP})
	LIST(APPEND Local
		HTTP.h
		HTTP.cpp
	)
	
	
	IF(${WIN32})
		ADD_DEFINITIONS(-DCURL_STATICLIB)
		
		LIST(APPEND OptimizedLibs ${CMAKE_SOURCE_DIR}/Source/External/curl/libcurl.lib)
		LIST(APPEND DebugLibs ${CMAKE_SOURCE_DIR}/Source/External/curl/libcurld.lib)
		
		LIST(APPEND Libs Ws2_32.lib wldap32.lib)
	ELSE()
		FIND_PACKAGE(CURL REQUIRED)
		LIST(APPEND Libs ${CURL_LIBRARIES})

		FIND_PACKAGE(Threads REQUIRED)
		LIST(APPEND Libs
			${CMAKE_THREAD_LIBS_INIT}
		)
	ENDIF()

		
ENDIF()
