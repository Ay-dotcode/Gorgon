#include "Graphics.h"
#include "VideoClip.h"


namespace gge { namespace multimedia {
	TheoraVideoManager *sVideoManager = NULL;
	OpenAL_AudioInterfaceFactory *sOpenALInterfaceFactory = NULL;

	TheoraOutputMode getTheoraColorMode(graphics::ColorMode::Type color_mode) {
		switch(color_mode) {
		case graphics::ColorMode::BGR:
			return TH_BGR;
		case graphics::ColorMode::RGB:
			return TH_RGB;
		case graphics::ColorMode::ABGR:
			return TH_ABGR;
		case graphics::ColorMode::ARGB:
			return TH_ARGB;
		default:
			return TH_RGB;
		}
	}

	VideoClip::VideoClip( const std::string &filename, bool cacheInMemory /*= true*/ ) : 
		ImageTexture(), FinishedEvent("Finished", this), mIsStarted(true), mIsLoaded(false), Loop(false), DataSource(NULL), mVideoClip(NULL)
	{
		mFilename = filename;
		mCacheInMemory = cacheInMemory;

		if(mFilename!="") {
			load();
		}
	}

	VideoClip::~VideoClip()
	{
		Destroy();
	}

	void VideoClip::LoadFile(const std::string &filename) {
		mFilename=filename;
		load();
	}

	void VideoClip::load() {
		if(mFilename=="") {
			throw std::runtime_error("No video file specified");
		}

		Destroy();

		if(mCacheInMemory) {
			DataSource = new TheoraMemoryFileDataSource(mFilename);
			mVideoClip = sVideoManager->createVideoClip(DataSource, getTheoraColorMode(graphics::ColorMode::RGB));
		}
		else {
			mVideoClip = sVideoManager->createVideoClip(mFilename, getTheoraColorMode(graphics::ColorMode::RGB));
		}

		if(!mVideoClip) {
			throw std::runtime_error("Cannot open video");
		}

		int width = mVideoClip->getWidth();
		int height = mVideoClip->getHeight();
		mDuration=int(mVideoClip->getDuration()*1000);
		mIsLoaded = true;

		if(Texture.ID==0) {
			Texture = graphics::system::GenerateTexture(NULL, width, height, graphics::ColorMode::RGB);
		}

		mRenderToken = gge::Main.BeforeRenderEvent.Register(this, &VideoClip::GetNextFrame);
	}

	void VideoClip::Destroy() {
		utils::CheckAndDelete(DataSource);

		if(mIsLoaded) {
			gge::Main.BeforeRenderEvent.Unregister(mRenderToken);
			mRenderToken=NULL;

			sVideoManager->destroyVideoClip(mVideoClip);

			mVideoClip=NULL;

			mIsLoaded = false;
		}
	}

	void VideoClip::GetNextFrame() {
		unsigned int time = Main.CurrentTime;

		float diff = (time-lastping) / 1000.0f;
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

		mVideoClip->update(diff);
		mVideoClip->decodedAudioCheck();

		lastping = time;

		if(IsFinished()) {
			if(Loop)
				Restart();
			else
				Destroy();

			FinishedEvent();
			
			return;
		}

		TheoraVideoFrame *frame = mVideoClip->getNextFrame();

		if(frame) {
			unsigned char* Data = frame->getBuffer();
			graphics::system::UpdateTexture(Texture, Data, graphics::ColorMode::RGB);

			mVideoClip->popFrame();
		}
	}

	void VideoClip::Play() {
		if(!mIsLoaded) {
			load();
		}

		lastping=Main.CurrentTime;
		mVideoClip->play();
	}

	void VideoClip::Pause(){
		if(!mVideoClip)
			return;

		mVideoClip->pause();
	}

	void VideoClip::Stop() {
		if(!mVideoClip)
			return;

		mVideoClip->stop();
	}

	void VideoClip::Restart() {
		if(!mIsLoaded) {
			load();
		}

		lastping=Main.CurrentTime;
		mVideoClip->restart();
	}

	void VideoClip::Seek(unsigned time) {
		if(!mVideoClip)
			return;

		if(time<GetPosition())
			mVideoClip->restart();

		mVideoClip->seek(time/1000.f, false);
	}

	void ReleaseSources() {
		utils::CheckAndDelete(sOpenALInterfaceFactory);
		utils::CheckAndDelete(sVideoManager);
	}

	void Initialize() {
		sVideoManager = new TheoraVideoManager(3);
		sVideoManager->setDefaultNumPrecachedFrames(32);

		sOpenALInterfaceFactory = new OpenAL_AudioInterfaceFactory();
		sVideoManager->setAudioInterfaceFactory(sOpenALInterfaceFactory);

		Main.BeforeTerminate.Register(&ReleaseSources);
	}

}}
