#pragma once

#include "OS.Win32.h"
#include "GGEMain.h"
#include "Image.h"

#include "../External/TheoraPlayback/TheoraVideoClip.h"
#include "../External/TheoraPlayback/TheoraPlayer.h"
#include "../External/TheoraPlayback/TheoraDataSource.h"
#include "../External/TheoraPlaybackOpenAL/OpenAL_AudioInterface.h"
#include "../External/TheoraPlayback/TheoraVideoManager.h"

namespace gge { namespace multimedia {

	void Initialize();

	class VideoClip : public virtual graphics::ImageTexture {
		friend class AudioClip;
	public:
		VideoClip(const std::string &filename="", bool cacheInMemory = false);
		virtual ~VideoClip();

		virtual void GetNextFrame();
		virtual void Destroy();
		utils::EventChain<VideoClip> FinishedEvent;

		virtual void LoadFile(const std::string &filename);

		virtual bool IsPaused() {
			return mVideoClip->isPaused();
		}

		virtual bool IsFinished() {
			return mVideoClip->isDone();
		}
		
		virtual void Play();
		virtual void Pause();
		virtual void Stop();
		virtual void Restart();
		virtual void Seek(unsigned time);


		virtual unsigned GetPosition() const {
			if(mIsLoaded)
				return int(mVideoClip->getTimePosition()*1000);
			else
				return 0;
		}

		virtual unsigned GetDuration() const {
			if(mIsLoaded)
				return mDuration;
			else
				return 0;
		}

		bool Loop;

	protected:
		virtual void load();

		TheoraVideoClip *mVideoClip;
		utils::ConsumableEvent<>::Token mRenderToken;

		std::string mFilename;
		bool mCacheInMemory;
		bool mIsStarted;
		bool mIsLoaded;

		TheoraMemoryFileDataSource *DataSource;

		unsigned int lastping;
		unsigned int mDuration;
	};
} }
