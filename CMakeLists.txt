cmake_minimum_required(VERSION 2.8)

PROJECT(GGE)

ADD_DEFINITIONS(-DLINUX -fPIC -O3)
SET(StandardFlags "-std=c++0x -Wall -Wno-reorder -Wno-unknown-pragmas -Wno-switch -Wno-unused-function")
SET(ExternalCxxFlags "-fpermissive")

SET(Utils
	Utils/Random.cpp
	Utils/SGuid.cpp
)

SET(Engine
	Engine/Image.cpp
	Engine/Sound.cpp
	Engine/Pointer.cpp
	Engine/OS.Linux.cpp
	Engine/Graphic2D.cpp
	Engine/GGEMain.cpp
	Engine/Graphics.cpp
	Engine/Input.cpp
	Engine/Animation.cpp
	Engine/CustomLayer.cpp
	Engine/Layer.cpp
	Engine/Font.cpp
	Engine/GraphicLayers.cpp
	Engine/OS.Win32.cpp
	Engine/OS.cpp
	Engine/OpenGL.cpp
	ShaderCode/ShaderCode.cpp
	Engine/ShaderBase.cpp
)

SET(Resource
	Resource/Image.cpp
	Resource/Sound.cpp
	Resource/Text.cpp
	Resource/Pointer.cpp
	Resource/Main.cpp
	Resource/Base.cpp
	Resource/Animation.cpp
	Resource/File.cpp
	Resource/BitmapFont.cpp
	Resource/LinkNode.cpp
	Resource/DataArray.cpp
	Resource/FontTheme.cpp
	Resource/Folder.cpp
)

SET(Widgets
	Widgets/Checkbox/CheckboxBlueprint.cpp
	Widgets/Checkbox/CheckboxBase.cpp
	Widgets/Dialogs/TextInput.cpp
	Widgets/Dialogs/Query.cpp
	Widgets/Dialogs/Message.cpp
	Widgets/Main.cpp
	Widgets/Basic/BorderData.cpp
	Widgets/Basic/Rectangle.cpp
	Widgets/Basic/Line.cpp
	Widgets/Basic/Region.cpp
	Widgets/Basic/ResizableObjectResource.cpp
	Widgets/Basic/Placeholder.cpp
	Widgets/Panel/PanelBase.cpp
	Widgets/Panel/PanelBlueprint.cpp
	Widgets/WidgetRegistry.cpp
	Widgets/Base/Widget.cpp
	Widgets/Base/Organizer.cpp
	Widgets/Tabpanel.cpp
	Widgets/Listbox/ListboxBlueprint.cpp
	Widgets/Slider/SliderBlueprint.cpp
	Widgets/Textbox/TextboxBlueprint.cpp
	Widgets/Textbox/TextboxBase.cpp
	Widgets/Combobox/ComboboxBlueprint.cpp
	Widgets/Organizers/LinearOrganizer.cpp
)

SET(Effects
	Effects/Tinting.cpp
	Effects/CountingText.cpp
	Effects/Flip.cpp
	Effects/LayerMover.cpp
	Effects/LayerResizer.cpp
)

SET(Encoding
	Encoding/LZMA.cpp
	Encoding/URI.cpp
	Encoding/PNG.cpp
	Encoding/JPEG.cpp
)

SET(Network
	Network/HTTP.cpp
)

SET(Multimedia
	Multimedia/Music.cpp
	Multimedia/Multimedia.cpp
	Multimedia/Wave.cpp
)

SET(EXT_LZMA
	External/LZMA/LzFind.c
	External/LZMA/LzmaDec.c
	External/LZMA/LzmaEnc.c
)

SET(EXT_PNG
	External/PNG/pngpread.c
	External/PNG/pngwutil.c
	External/PNG/pngwtran.c
	External/PNG/pngset.c
	External/PNG/pngrio.c
	External/PNG/pngmem.c
	External/PNG/png.c
	External/PNG/pngget.c
	External/PNG/pngrutil.c
	External/PNG/pngtest.c
	External/PNG/pngtrans.c
	External/PNG/pngerror.c
	External/PNG/pngwio.c
	External/PNG/pngwrite.c
	External/PNG/pngrtran.c
	External/PNG/pngread.c
)

SET(EXT_ZLib
	External/zlib/compress.c
	External/zlib/gzlib.c
	External/zlib/uncompr.c
	External/zlib/gzclose.c
	External/zlib/deflate.c
	External/zlib/gzread.c
	External/zlib/inflate.c
	External/zlib/crc32.c
	External/zlib/trees.c
	External/zlib/inftrees.c
	External/zlib/inffast.c
	External/zlib/adler32.c
	External/zlib/gzwrite.c
	External/zlib/zutil.c
	External/zlib/infback.c
)

SET(EXT_JPEG
	External/JPEG/src/jdinput.c
	External/JPEG/src/jdhuff.c
	External/JPEG/src/jdmainct.c
	External/JPEG/src/jfdctint.c
	External/JPEG/src/jdmarker.c
	External/JPEG/src/jchuff.c
	External/JPEG/src/jidctint.c
	External/JPEG/src/jdtrans.c
	External/JPEG/src/jccoefct.c
	External/JPEG/src/jdcoefct.c
	External/JPEG/src/jcmainct.c
	External/JPEG/src/jccolor.c
	External/JPEG/src/jctrans.c
	External/JPEG/src/jfdctflt.c
	External/JPEG/src/jcomapi.c
	External/JPEG/src/jdapistd.c
	External/JPEG/src/jcprepct.c
	External/JPEG/src/jidctflt.c
	External/JPEG/src/jquant2.c
	External/JPEG/src/jcmaster.c
	External/JPEG/src/jidctred.c
	External/JPEG/src/jidctfst.c
	External/JPEG/src/jerror.c
	External/JPEG/src/jcinit.c
	External/JPEG/src/jdapimin.c
	External/JPEG/src/jdmerge.c
	External/JPEG/src/jmemansi.c
	External/JPEG/src/jddctmgr.c
	External/JPEG/src/jcmarker.c
	External/JPEG/src/jcdctmgr.c
	External/JPEG/src/jdatadst.c
	External/JPEG/src/jcphuff.c
	External/JPEG/src/jmemmgr.c
	External/JPEG/src/jdmaster.c
	External/JPEG/src/jcapistd.c
	External/JPEG/src/jdphuff.c
	External/JPEG/src/jquant1.c
	External/JPEG/src/jutils.c
	External/JPEG/src/jcparam.c
	External/JPEG/src/jdatasrc.c
	External/JPEG/src/jdcolor.c
	External/JPEG/src/jfdctfst.c
	External/JPEG/src/jcapimin.c
	External/JPEG/src/jdpostct.c
	External/JPEG/src/jdsample.c
	External/JPEG/src/jcsample.c
)

SET(EXT_OGG
	External/ogg/src/framing.c
	External/ogg/src/bitwise.c
)

SET(EXT_Vorbis
	External/vorbis/src/floor0.c
	External/vorbis/src/psy.c
	External/vorbis/src/analysis.c
	External/vorbis/src/sharedbook.c
	External/vorbis/src/lpc.c
	External/vorbis/src/bitrate.c
	External/vorbis/src/mdct.c
	External/vorbis/src/registry.c
	External/vorbis/src/codebook.c
	External/vorbis/src/vorbisfile.c
	External/vorbis/src/lsp.c
	External/vorbis/src/synthesis.c
	External/vorbis/src/envelope.c
	External/vorbis/src/mapping0.c
	External/vorbis/src/block.c
	External/vorbis/src/window.c
	External/vorbis/src/res0.c
	External/vorbis/src/lookup.c
	External/vorbis/src/info.c
	External/vorbis/src/vorbisenc.c
	External/vorbis/src/floor1.c
	External/vorbis/src/smallft.c
)

SET(EXT_Xml
	External/XmlParser/tinyxml.cpp
	External/XmlParser/tinyxmlerror.cpp
	External/XmlParser/tinyxmlparser.cpp
)

SET(Externals
	${EXT_PNG}
	${EXT_ZLib}
	${EXT_JPEG}
	${EXT_LZMA}
	${EXT_Xml}
	${EXT_OGG}
	${EXT_Vorbis}
	External/glutil/MatrixStack.cpp
)

FOREACH(_file ${Utils} ${Engine} ${Resource} ${Widgets} ${Effects} ${Encoding} ${Network} ${Multimedia})
	SET_SOURCE_FILES_PROPERTIES(${_file} PROPERTIES COMPILE_FLAGS ${StandardFlags})
ENDFOREACH()

FOREACH(_file ${EXT_Xml})
	SET_SOURCE_FILES_PROPERTIES(${_file} PROPERTIES COMPILE_FLAGS ${ExternalCxxFlags})
ENDFOREACH()

FOREACH(_file ${EXT_LZMA})
	SET_SOURCE_FILES_PROPERTIES(${_file} PROPERTIES COMPILE_FLAGS "-D_7ZIP_ST")
ENDFOREACH()


SET(sources
	${Utils}
	${Engine}
	${Resource}
	${Widgets}
	${Effects}
	${Encoding}
	${Network}
	${Multimedia}
)

SET(libraries
	GL
	openal
	X11
	pthread
	rt
)
ADD_LIBRARY(GGE STATIC ${sources} ${Externals})

FOREACH(_lib ${libraries})
	FIND_LIBRARY(_libn ${_lib})
	ADD_LIBRARY(${_lib} UNKNOWN IMPORTED)
	SET_TARGET_PROPERTIES(${_lib} PROPERTIES IMPORTED_LOCATION ${_libn})
	TARGET_LINK_LIBRARIES(GGE ${_lib})
	UNSET(_libn CACHE)
ENDFOREACH()
