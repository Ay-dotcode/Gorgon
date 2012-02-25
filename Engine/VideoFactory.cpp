#include "VideoFactory.h"
#include "Graphics.h"
#include "VideoTexture.h"
#include "..\External\TheoraPlayback\TheoraVideoClip.h"
#include "..\External\TheoraPlayback\TheoraPlayer.h"
#include "..\External\TheoraPlayback\TheoraDataSource.h"
#include "..\External\TheoraPlayback\OpenAL_AudioInterface.h"
#include "..\External\TheoraPlayback\TheoraVideoManager.h"
#include <stdexcept>

// Eser: autoRestart param should be somewhere else
// maybe we should consider making it a class property.
namespace gge { namespace graphics {
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

	VideoTexture &VideoFactory::OpenFile(std::string filename, bool audioOnly, ColorMode::Type colorMode, bool autoRestart, bool cacheInMemory) {
		// Eser: RGB mode is hardcoded. needs to be defined as a property instead.
		TheoraVideoClip *videoClip;

		if(cacheInMemory) {
			TheoraMemoryFileDataSource *DataSource = new TheoraMemoryFileDataSource(filename);
			videoClip = VideoManager.createVideoClip(DataSource, getTheoraColorMode(colorMode));
		}
		else {
			videoClip = VideoManager.createVideoClip(filename, getTheoraColorMode(colorMode));
		}

		if(!videoClip) {
			throw std::runtime_error("Cannot open video");
		}

		if(autoRestart) {
			// videoClip->setAutoRestart(1);
		}

		return *new VideoTexture(*this, videoClip);
	}

	VideoFactory::VideoFactory() : VideoManager(*(new TheoraVideoManager(3))), OpenALInterfaceFactory(*(new OpenAL_AudioInterfaceFactory()))
	{
		VideoManager.setDefaultNumPrecachedFrames(32);
		VideoManager.setAudioInterfaceFactory(&OpenALInterfaceFactory);
	}

	VideoFactory::~VideoFactory()
	{
		delete &OpenALInterfaceFactory;
		delete &VideoManager;
	}


	/*void VideoTexture::GetNextFrame() {
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
	}*/
}}
