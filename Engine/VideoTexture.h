#pragma once

#include "OS.Win32.h"
#include "GGEMain.h"
#include "Image.h"

class TheoraVideoManager;
class TheoraVideoClip;
class OpenAL_AudioInterfaceFactory;

namespace gge { namespace graphics {
	class VideoFactory;

	class VideoTexture : public virtual ImageTexture {
	public:
		VideoTexture(VideoFactory &videoFactory, TheoraVideoClip *videoClip, bool audioOnly = false, ColorMode::Type colorMode = ColorMode::RGB, bool autoRestart = false, bool cacheInMemory = true);

		~VideoTexture();

		//void drawto(ImageTarget2D& Target, const SizeController2D &controller, int X, int Y, int W, int H);

		virtual void GetNextFrame();

		VideoFactory &VideoFactoryRef;

	protected:
		TheoraVideoClip *mVideoClip;
		utils::ConsumableEvent<>::Token mRenderToken;
		bool mAudioOnly;
		ColorMode::Type mColorMode;
		bool mAutoRestart;
		bool mCacheInMemory;
	};
} }
