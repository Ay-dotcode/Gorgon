#pragma once

#include "OS.Win32.h"
#include "GGEMain.h"
#include "Image.h"

class TheoraVideoManager;
class TheoraVideoClip;
class OpenAL_AudioInterfaceFactory;

namespace gge { namespace graphics {
	class VideoClip : public virtual ImageTexture {
	public:
		VideoClip(TheoraVideoClip *videoClip, bool audioOnly = false, ColorMode::Type colorMode = ColorMode::RGB, bool autoRestart = false, bool cacheInMemory = true);
		~VideoClip();

		virtual void GetNextFrame();

		static TheoraVideoManager *sVideoManager;

	protected:
		TheoraVideoClip *mVideoClip;
		utils::ConsumableEvent<>::Token mRenderToken;
		utils::EventChain<VideoClip> FinishedEvent;
		bool mAudioOnly;
		ColorMode::Type mColorMode;
		bool mAutoRestart;
		bool mCacheInMemory;
	};
} }
