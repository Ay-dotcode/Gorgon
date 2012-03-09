#include "Graphics.h"
#include "VideoClip.h"
#include "..\External\TheoraPlayback\TheoraVideoClip.h"
#include "..\External\TheoraPlayback\TheoraPlayer.h"
#include "..\External\TheoraPlayback\TheoraDataSource.h"
#include "..\External\TheoraPlaybackOpenAL\OpenAL_AudioInterface.h"
#include "..\External\TheoraPlayback\TheoraVideoManager.h"

namespace gge { namespace graphics {
	TheoraVideoManager *VideoClip::sVideoManager = NULL;
	OpenAL_AudioInterfaceFactory *VideoClip::sOpenALInterfaceFactory = NULL;

	TheoraOutputMode getTheoraColorMode(ColorMode::Type color_mode) {
		switch(color_mode) {
		case ColorMode::BGR:
			return TH_BGR;
		case ColorMode::RGB:
			return TH_RGB;
		case ColorMode::ABGR:
			return TH_ABGR;
		case ColorMode::ARGB:
			return TH_ARGB;
		default:
			return TH_RGB;
		}
	}

	VideoClip::VideoClip( std::string filename, bool audioOnly /*= false*/, ColorMode::Type colorMode /*= ColorMode::RGB*/, bool cacheInMemory /*= true*/ ) : 
		ImageTexture(), OnFinished("Finished", this), mIsStarted(true), mIsLoaded(false)
	{
		mFilename = filename;
		mCacheInMemory = cacheInMemory;
		mAudioOnly = audioOnly;
		mColorMode = colorMode;

		if(!sVideoManager) {
			sVideoManager = new TheoraVideoManager(3);
			sVideoManager->setDefaultNumPrecachedFrames(32);

			sOpenALInterfaceFactory = new OpenAL_AudioInterfaceFactory();
			sVideoManager->setAudioInterfaceFactory(sOpenALInterfaceFactory);
		}

		load();
	}

	VideoClip::~VideoClip()
	{
		Destroy();
	}

	void VideoClip::load() {
		if(mCacheInMemory) {
			TheoraMemoryFileDataSource *DataSource = new TheoraMemoryFileDataSource(mFilename);
			mVideoClip = sVideoManager->createVideoClip(DataSource, getTheoraColorMode(mColorMode));
		}
		else {
			mVideoClip = sVideoManager->createVideoClip(mFilename, getTheoraColorMode(mColorMode));
		}

		if(!mVideoClip) {
			throw std::runtime_error("Cannot open video");
		}

		mDuration = mVideoClip->getDuration();
		mWidth = mVideoClip->getWidth();
		mHeight = mVideoClip->getHeight();
		mIsLoaded = true;

		// Eser: fixme if graphics::system::GenerateTexture resets/will reset the existing W/H values of GLTexture struct.
		if(!mAudioOnly) {
			Texture = graphics::system::GenerateTexture(NULL, mWidth, mHeight, ColorMode::RGB);
		}

		mRenderToken = gge::Main.BeforeRenderEvent.Register(this, &VideoClip::GetNextFrame);
	}

	void VideoClip::Destroy() {
		gge::Main.BeforeRenderEvent.Unregister(mRenderToken);

		VideoClip::sVideoManager->destroyVideoClip(mVideoClip);

		mIsLoaded = false;

		// Eser: fixme
		// gge::utils::CheckAndDelete(mVideoClip);
	}

	void VideoClip::GetNextFrame() {
		// Eser: not sure using statics to measure the past time the right decision here
		// i'd rather having a different timer which calculates the time diff between last & current frames.
		static unsigned int time = gge::Main.CurrentTime;
		unsigned int t = Main.CurrentTime;

		float diff = (t-time) / 1000.0f;
		if (diff > 0.25f) {
			diff = 0.05f; // prevent spikes (usually happen on app load)
		}

		if(!mVideoClip) {
			return;
		}

		if(mIsStarted) {
			if(mVideoClip->getNumReadyFrames() < mVideoClip->getNumPrecachedFrames() * 0.5f ) {
				return;
			}

			mIsStarted = false;
		}

		if(!mAudioOnly) {
			mVideoClip->update(diff);
		}
		mVideoClip->decodedAudioCheck();

		time = t;

		if(IsFinished()) {
			Destroy();
			OnFinished();
			return;
		}

		TheoraVideoFrame *frame = mVideoClip->getNextFrame();

		if(frame) {
			unsigned char* Data = frame->getBuffer();
			if(!mAudioOnly) {
				graphics::system::UpdateTexture(Texture, Data, ColorMode::RGB);
			}

			mVideoClip->popFrame();
			// delete frame;
			// delete Data;
		}
	}

	bool VideoClip::IsPaused() {
		return mVideoClip->isPaused();
	}

	bool VideoClip::IsFinished() {
		return mVideoClip->isDone();
	}

	void VideoClip::Play() {
		if(!mIsLoaded) {
			load();
		}

		mVideoClip->play();
	}

	void VideoClip::Pause(){
		mVideoClip->pause();
	}

	void VideoClip::Stop() {
		mVideoClip->stop();
	}

	void VideoClip::Restart() {
		if(!mIsLoaded) {
			load();
		}

		mVideoClip->restart();
	}

	void VideoClip::Seek(float time) {
		mVideoClip->seek(time, false);
	}

	float VideoClip::GetPosition() {
		return mVideoClip->getTimePosition();
	}

	float VideoClip::GetLength() {
		return mDuration;
	}

	int VideoClip::GetWidth() {
		return mWidth;
	}

	int VideoClip::GetHeight() {
		return mHeight;
	}
}}
