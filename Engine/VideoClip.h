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
		VideoClip(std::string filename, bool audioOnly = false, ColorMode::Type colorMode = ColorMode::RGB, bool cacheInMemory = true);
		~VideoClip();

		virtual void GetNextFrame();
		virtual void Destroy();
		utils::EventChain<VideoClip> OnFinished;

		virtual bool IsPaused();
		virtual bool IsFinished();
		virtual void Play();
		virtual void Pause();
		virtual void Stop();
		virtual void Restart();
		virtual void Seek(float time);
		virtual float GetPosition();
		virtual float GetLength();
		virtual int GetWidth();
		virtual int GetHeight();

		static void ReleaseSources() {
			delete &sOpenALInterfaceFactory;
			delete &sVideoManager;
		}

	protected:
		static TheoraVideoManager *sVideoManager;
		static OpenAL_AudioInterfaceFactory *sOpenALInterfaceFactory;

		virtual void load();

		TheoraVideoClip *mVideoClip;
		utils::ConsumableEvent<>::Token mRenderToken;
		bool mAudioOnly;
		ColorMode::Type mColorMode;

		std::string mFilename;
		bool mCacheInMemory;
		bool mIsStarted;
		bool mIsLoaded;

		float mDuration;
		int mWidth;
		int mHeight;
	};
} }
