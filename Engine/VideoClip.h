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
		VideoClip(std::string filename, bool audioOnly = false, ColorMode::Type colorMode = ColorMode::RGB, bool autoRestart = false, bool cacheInMemory = true);
		~VideoClip();

		virtual void GetNextFrame();
		static void Destroy() {
			delete &sOpenALInterfaceFactory;
			delete &sVideoManager;
		}

	protected:
		static TheoraVideoManager *sVideoManager;
		static OpenAL_AudioInterfaceFactory *sOpenALInterfaceFactory;

		TheoraVideoClip *mVideoClip;
		utils::ConsumableEvent<>::Token mRenderToken;
		utils::EventChain<VideoClip> FinishedEvent;
		bool mAudioOnly;
		ColorMode::Type mColorMode;
		bool mAutoRestart;
		bool mCacheInMemory;
	};
} }
