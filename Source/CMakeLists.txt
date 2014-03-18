cmake_minimum_required(VERSION 2.8)

ADD_SUBDIRECTORY(Filesystem)

SET(Source
	Source/Filesystem.h
	Source/Threading.h
	
	PARENT_SCOPE
)

SOURCE_GROUP(Source FILES ${SOURCE})

SET(Source
	${Filesystem}
	${Source}
)
