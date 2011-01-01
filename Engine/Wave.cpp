#include "Wave.h"

namespace gge {
	////For garbage collection
	static Collection<Wave> Waves;

	Wave::Wave(SoundBufferHandle buffer, int maxWaveDistance) {
		buffer=buffer;
		isavailable=false;
		AutoDestruct=true;
		finished=NULL;
		finishedstateisknown=0;

		if(maxWaveDistance)
			isavailable= (bool)(source=Create3DSoundController(buffer, maxWaveDistance));
		else
			isavailable= (bool)(source=CreateSoundController(buffer));

		Waves.Add(this);
	}

	void WaveGarbageCollect_fire(empty_event_params params, GGEMain &main, Any data, string eventname) {
		CollectWaveGarbage();
	}

	void InitWaveGarbageCollect(GGEMain *main) {
		main->AfterRenderEvent.Register(WaveGarbageCollect_fire);
	}

	Wave* Wave::Play() {
		finishedstateisknown=0;
		
		alSourcei(source,AL_LOOPING,0);
		alSourcePlay(source);
		return this;
	}

	Wave::~Wave() {
		_destroy();
	}
	
	Wave* Wave::Loop() {
		finishedstateisknown=0;
		alSourcei(source, AL_LOOPING, 1);
		alSourcePlay(source);
		return this;
	}
	
	Wave* Wave::SetVolume(float Volume) {
		alSourcef(source, AL_GAIN, Volume);

		return this;
	}
	
	Wave* Wave::Set3DPosition(float x,float y,float z) {
		alSource3f(source, AL_POSITION, x, y, z);

		return this;
	}
	
	Wave* Wave::Stop() {
		alSourceStop(source);
		return this;
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

	void CollectWaveGarbage() {
		Waves.ResetIteration();
		Wave *wave;

		while(wave=Waves.next()) {
			if(wave->AutoDestruct) {
				if(!wave->isPlaying()) {
					Waves.Remove(wave);
					wave->_destroy();
					delete wave;
					wave=NULL;
				}
			}
		}
	}
}
