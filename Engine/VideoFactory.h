#pragma once

#include "OS.Win32.h"
#include "GGEMain.h"
#include "Image.h"

class TheoraVideoManager;
class TheoraVideoClip;
class OpenAL_AudioInterfaceFactory;

namespace gge { namespace graphics {
	class VideoTexture;

	class VideoFactory {
	public:
		VideoFactory();

		~VideoFactory();

		//void drawto(ImageTarget2D& Target, const SizeController2D &controller, int X, int Y, int W, int H);

		virtual VideoTexture* OpenFile(std::string filename, bool audioOnly = false, ColorMode::Type colorMode = ColorMode::RGB, bool autoRestart = false, bool cacheInMemory = true);

		TheoraVideoManager &VideoManager;
		OpenAL_AudioInterfaceFactory &OpenALInterfaceFactory;
	};
} }
