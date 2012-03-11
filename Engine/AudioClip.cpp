#include "VideoClip.h"
#include "AudioClip.h"


namespace gge { namespace multimedia {

	extern TheoraVideoManager *sVideoManager;
	extern OpenAL_AudioInterfaceFactory *sOpenALInterfaceFactory;


	AudioClip::AudioClip( const std::string &filename, bool cacheInMemory /*= true*/ ) : 
	FinishedEvent("Finished", this), mIsStarted(true), mIsLoaded(false), Loop(false)
	{
		mFilename = filename;
		mCacheInMemory = cacheInMemory;

		if(mFilename!="") {
			load();
		}
	}

	AudioClip::~AudioClip()
	{
		Destroy();
	}

	void AudioClip::LoadFile(const std::string &filename) {
		mFilename=filename;
		load();
	}

	void AudioClip::load() {
		if(mFilename=="") {
			throw std::runtime_error("No audio file specified");
		}

		if(mCacheInMemory) {
			TheoraMemoryFileDataSource *DataSource = new TheoraMemoryFileDataSource(mFilename);
			mVideoClip = sVideoManager->createVideoClip(DataSource);
		}
		else {
			mVideoClip = sVideoManager->createVideoClip(mFilename);
		}

		if(!mVideoClip) {
			throw std::runtime_error("Cannot open audio");
		}

		mIsLoaded = true;

		mRenderToken = gge::Main.BeforeRenderEvent.Register(this, &AudioClip::GetNextFrame);
	}

	void AudioClip::Destroy() {
		if(mIsLoaded) {
			gge::Main.BeforeRenderEvent.Unregister(mRenderToken);

			sVideoManager->destroyVideoClip(mVideoClip);

			mIsLoaded = false;
		}
	}

	void AudioClip::GetNextFrame() {
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
			Destroy();
			FinishedEvent();

			if(Loop)
				Restart();

			return;
		}

		TheoraVideoFrame *frame = mVideoClip->getNextFrame();

		if(frame) {
			unsigned char* Data = frame->getBuffer();

			mVideoClip->popFrame();
		}
	}

	void AudioClip::Play() {
		if(!mIsLoaded) {
			load();
		}

		lastping=Main.CurrentTime;
		mVideoClip->play();
	}

	void AudioClip::Pause(){
		mVideoClip->pause();
	}

	void AudioClip::Stop() {
		mVideoClip->stop();
	}

	void AudioClip::Restart() {
		if(!mIsLoaded) {
			load();
		}

		lastping=Main.CurrentTime;
		mVideoClip->restart();
	}

	void AudioClip::Seek(unsigned time) {
		mVideoClip->seek(time/1000.f, false);
	}
}}
