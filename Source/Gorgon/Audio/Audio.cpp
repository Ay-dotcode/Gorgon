#include <cstring>

#include "../Audio.h"

#include "Controllers.h"
	
#ifdef AUDIO_PULSE
#	include "PulseAudio.inc.h"
#endif

namespace Gorgon { namespace Audio {
	
	Utils::Logger Log("Audio");
	
	std::vector<Device> Device::devices;
	Device Current;
	
	Device Device::def;
	
	namespace internal {
		std::thread audiothread;
		
		int BufferSize = 128;
		
		float mastervolume = 1;
		std::vector<float> volume = {1.f, 1.f};
	}
	
	void SetVolume(float volume) {
		internal::mastervolume = volume;
	}

	void SetVolume(Channel channel, float volume) {
		for(int i=0;i<Current.GetChannelCount(); i++) {
			if(Current.GetChannel(i) == channel) {
				internal::volume[i] = volume;
				return;
			}
		}
		
		if(channel == Channel::Mono) {
			for(int i=0;i<Current.GetChannelCount(); i++) {
				internal::volume[i] = volume;
			}
		}
	}
	
	float GetVolume() {
		return internal::mastervolume;
	}

	float GetVolume(Channel channel) {
		for(int i=0;i<Current.GetChannelCount(); i++) {
			if(Current.GetChannel(i) == channel)
				return internal::volume[i];
		}
		
		return 0;
	}
	
	void AudioLoop() {
		Log << "Audio loop started";
		
		int channels = Device::Default().GetChannelCount();
		int datasize = channels * internal::BufferSize;
		
		float data[datasize];
		int   freq 		   = Device::Default().GetSampleRate();
		float secpersample = 1.0f / freq;
		
		while(true) {
			
			
	
			int maxsize, size;

			maxsize = GetWritableSize();
			size    = maxsize / sizeof(float) / channels;
			size    = std::min(size, internal::BufferSize);

			if(!size) goto end;
			
			//zero out before starting
			std::memset(data, 0, sizeof(float) * datasize);
			
			internal::ControllerMtx.lock();
			
			//For cache optimization we would do vertical sampling
			for(auto &controller : internal::Controllers) {
				if(controller.Type() == ControllerType::Basic) {
					BasicController &basic = static_cast<BasicController&>(controller);
					
					//Log<<basic.position<<"/"<<basic.GetDuration()<<": "<<basic.playing;
					
					if(!basic.playing) continue;
					
					if(!basic.wavedata) continue;
					
					if(basic.wavedata->GetSize() < internal::BufferSize) continue;
					
					auto wavedata = basic.wavedata;
					
					auto outofbounds = [&]() {
						//reached to the end of the stream
						if(basic.looping) {
							basic.position = 0;
							
							return true;
						}
						else {
							basic.position = basic.GetDuration();
							basic.playing = false;
							
							return false;
						}
					};
					
					if(wavedata->GetChannelCount() == 1) {
						for(int s=0; s<size; s++) {
							
						recalculate:
							float pos     = basic.position * wavedata->GetSampleRate();
							
							int x1 = pos    ;
							int x2 = pos + 1;
							
							if(x2 >= wavedata->GetSize()) {
								if(outofbounds()) goto recalculate;
								else break;
							}
							
							float x1r = x2 - pos;
							float x2r = 1  - x1r;
							
							float val = internal::mastervolume * basic.volume * ( x1r * wavedata->Get(x1, 0) + x2r * wavedata->Get(x2, 0));
							
							for(int c = 0; c<channels; c++) {
								data[s*channels+c] = internal::volume[c] * val;
							}
							
							basic.position += secpersample;
						}
					}
				}
			}
			
			internal::ControllerMtx.unlock();
			
end:
			PostData(data, size*channels*sizeof(float));
			std::this_thread::yield();
		}
	}
} }