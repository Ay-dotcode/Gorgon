
SET(Local
	LzFind.c
	LzmaDec.c
	LzmaEnc.c
)

IF(${WIN32})
	LIST(APPEND Local
		LzFindMt.c
		Threads.c
	)
ELSE() 
	ADD_DEFINITIONS(-D_7ZIP_ST)
ENDIF()