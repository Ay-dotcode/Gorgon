#include "VideoFactory.h"
#include "Graphics.h"
#include "VideoTexture.h"
#include "..\External\TheoraPlayback\TheoraVideoClip.h"
#include "..\External\TheoraPlayback\TheoraPlayer.h"
#include "..\External\TheoraPlayback\TheoraDataSource.h"
#include "..\External\TheoraPlayback\OpenAL_AudioInterface.h"
#include "..\External\TheoraPlayback\TheoraVideoManager.h"

// Eser: autoRestart param should be somewhere else
// maybe we should consider making it a class property.
namespace gge { namespace graphics {
	void VideoTexture::GetNextFrame() {
		// Eser: not sure using statics to measure the past time the right decision here
		// i'd rather having a different timer which calculates the time diff between last & current frames.
		static unsigned int time = Main.CurrentTime;
		unsigned int t = Main.CurrentTime;

		float diff = (t-time) / 1000.0f;
		if (diff > 0.25f) {
			diff = 0.05f; // prevent spikes (usually happen on app load)
		}

		if(!mVideoClip) {
			return;
		}

		if(!mAudioOnly) {
			mVideoClip->update(diff);
		}
		mVideoClip->decodedAudioCheck();

		time = t;

		//if(mVideoClip->getNumReadyFrames() < mVideoClip->getNumPrecachedFrames() * 0.5f ) {
		//	return;
		//}

		TheoraVideoFrame *frame = mVideoClip->getNextFrame();

		if(frame) {
			unsigned char* Data = frame->getBuffer();
			if(!mAudioOnly) {
				//!Fix me
				//graphics::system::UpdateTexture(Data, ColorMode::RGB, Texture);
			}

			mVideoClip->popFrame();
	//				delete frame;
		}
	}

	VideoTexture::~VideoTexture()
	{
		Main.BeforeRenderEvent.Unregister(mRenderToken);

		VideoFactoryRef.VideoManager.destroyVideoClip(mVideoClip);
		gge::utils::CheckAndDelete(mVideoClip);
	}

	VideoTexture::VideoTexture( VideoFactory &videoFactory, TheoraVideoClip *videoClip, bool audioOnly /*= false*/, ColorMode::Type colorMode /*= ColorMode::RGB*/, bool autoRestart /*= false*/, bool cacheInMemory /*= true*/ ) : ImageTexture(), VideoFactoryRef(videoFactory)
	{
		mVideoClip = videoClip;

		mAudioOnly = audioOnly;
		mColorMode = colorMode;
		mAutoRestart = autoRestart;
		mCacheInMemory = cacheInMemory;

		// Eser: fixme if graphics::system::GenerateTexture resets/will reset the existing W/H values of GLTexture struct.
		if(!mAudioOnly) {
			Texture = graphics::system::GenerateTexture(NULL, mVideoClip->getWidth(), mVideoClip->getHeight(), ColorMode::RGB);
		}

		mRenderToken = Main.BeforeRenderEvent.Register(this, &VideoTexture::GetNextFrame);
	}

}}
