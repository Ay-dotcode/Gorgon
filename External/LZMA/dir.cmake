
SET(Local
	LzFind.h
	LzHash.h
	LzmaDec.h
	LzmaEnc.h
	Threads.h
	Types.h
	LzFind.c
	LzmaDec.c
	LzmaEnc.c
)

IF(${WIN32})
	LIST(APPEND Local
		LzFindMt.h
		LzFindMt.c
		Threads.c
	)
ELSE() 
	ADD_DEFINITIONS(-D_7ZIP_ST)
ENDIF()
