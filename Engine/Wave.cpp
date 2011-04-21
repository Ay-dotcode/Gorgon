#include "Wave.h"

namespace gge { namespace sound {

	namespace system {
		////For garbage collection
		static utils::Collection<Wave> Waves;
		void InitWaveGarbageCollect(GGEMain *main) {
			main->AfterRenderEvent.Register(CollectWaveGarbage);
		}

		void CollectWaveGarbage() {
			Waves.ResetIteration();
			Wave *wave;

			while(wave=Waves.next()) {
				if(!wave->isPlaying()) {
					if(wave->AutoDestruct) {
						Waves.Remove(wave);
						delete wave;
					} else {
						if(wave->finished) wave->finished(*wave);
					}
				}
			}
		}
	}

	Wave::Wave(system::SoundBufferHandle buffer, float maxWaveDistance) {
		buffer=buffer;
		isavailable=false;
		AutoDestruct=true;
		finished=NULL;
		finishedstateisknown=0;

		if(maxWaveDistance)
			isavailable= (source=system::Create3DSoundController(buffer, maxWaveDistance)) != 0;
		else
			isavailable= (source=system::CreateSoundController(buffer)) != 0;

		system::Waves.Add(this);
	}

	Wave& Wave::Play() {
		finishedstateisknown=0;

		alSourcei(source,AL_LOOPING,0);
		alSourcePlay(source);
		return *this;
	}

	Wave::~Wave() {
		_destroy();
	}

	Wave& Wave::Loop() {
		finishedstateisknown=0;
		alSourcei(source, AL_LOOPING, 1);
		alSourcePlay(source);
		return *this;
	}

	Wave& Wave::SetVolume(float Volume) {
		alSourcef(source, AL_GAIN, Volume);

		return *this;
	}

	Wave& Wave::Set3DPosition(float x,float y,float z) {
		alSource3f(source, AL_POSITION, x, y, z);

		return *this;
	}

	Wave& Wave::Stop() {
		alSourceStop(source);
		return *this;
	}

	void Wave::_destroy() {
		isavailable=false;
		alDeleteSources(1,&source);
		source=0;
	}

	bool Wave::isPlaying() {
		int status=0;
		alGetSourcei(source,AL_SOURCE_STATE,&status);
		return status==AL_PLAYING;
	}

} }
