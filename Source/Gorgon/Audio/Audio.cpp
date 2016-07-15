#include <cstring>

#include "../Audio.h"

#include "Controllers.h"
	
#ifdef AUDIO_PULSE
#	include "PulseAudio.inc.h"
#endif
#ifdef AUDIO_WASAPI
#	include "WASAPI.inc.h"
#endif

namespace Gorgon { namespace Audio {
	
	Utils::Logger Log("Audio");
	
	std::vector<Device> Device::devices;
	Device Current;
	
	Device Device::def;
	
	namespace internal {
		std::thread audiothread;
		
		int BufferSize = 400;
		
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
		
		int channels = Current.GetChannelCount();
		int datasize = channels * internal::BufferSize;
		
		float *data  = (float*)malloc(datasize*sizeof(float));

		int   freq 		   = Current.GetSampleRate();
		float secpersample = 1.0f / freq;
		
		std::shared_ptr<float> tempmemory(data, free);

		//std::ofstream test("test.csv");
		
		while(true) {
			int maxsize, size;

			maxsize = GetWritableSize();
			size    = maxsize / sizeof(float) / channels;
			size    = std::min(size, internal::BufferSize);

			if(!size) {
				SkipFrame();
				std::this_thread::yield();
				continue;
			}
			
			//zero out before starting
			std::memset(data, 0, sizeof(float) * datasize);
			
			internal::ControllerMtx.lock();
			
			//For cache optimization we would do vertical sampling
			for(auto &controller : internal::Controllers) {
				if(controller.Type() == ControllerType::Basic) {
					BasicController &basic = static_cast<BasicController&>(controller);
					
					//std::cout<<"Playing.."<<std::endl;
					
					if(!basic.playing) continue;
					
					if(!basic.wavedata) continue;
					
					if((int)basic.wavedata->GetSize() < internal::BufferSize) continue;
					
					auto wavedata = basic.wavedata;
					
                    auto org = basic.position;
					
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
                    
                    auto distributetoall = [&](int ind) {
                        basic.position = org;
                        
                        for(int s=0; s<size; s++) {
                            
                        recalculate:
                            float pos     = basic.position * wavedata->GetSampleRate();
                            
                            int x1 = (int)pos      ;
                            int x2 = (int)(pos + 1);
                            
                            if(x1 >= (int)wavedata->GetSize()) {
                                if(outofbounds()) goto recalculate;
                                else break;
                            }
                            
                            float x1r = x2 - pos;
                            float x2r = 1  - x1r;
                            
                            float val;
                            
                            if(x2 >= (int)wavedata->GetSize()) {
                                // multiplication with basic.looping is for branch elimination. Basically, if the audio is not looping
                                // last few values will not be interpolated but faded out.
                                val = internal::mastervolume * basic.volume * ( x1r * wavedata->Get(x1, ind) + basic.looping * x2r * wavedata->Get(0, ind));
                            }
                            else {
                                val = internal::mastervolume * basic.volume * ( x1r * wavedata->Get(x1, ind) + x2r * wavedata->Get(x2, ind));
                            }
                            
                            for(int c = 0; c<channels; c++) {
                                data[s*channels+c] += internal::volume[c] * val;
                            }
                            
                            basic.position += secpersample;
                        }
                    };
                    
                    //optimization might be necessary, this function may cause cache misses for many to many mapping
                    auto sendto = [&](int src, int dest) {
                        basic.position = org;
                        
                        for(int s=0; s<size; s++) {
                            
                        recalculate:
                            float pos     = basic.position * wavedata->GetSampleRate();
                            
                            int x1 = (int)pos      ;
                            int x2 = (int)(pos + 1);
                            
                            if(x1 >= (int)wavedata->GetSize()) {
                                if(outofbounds()) goto recalculate;
                                else break;
                            }
                            
                            float x1r = x2 - pos;
                            float x2r = 1  - x1r;
                            
                            float val;
                            
                            if(x2 >= (int)wavedata->GetSize()) {
                                // multiplication with basic.looping is for branch elimination. Basically, if the audio is not looping
                                // last few values will not be interpolated but faded out.
                                val = ( x1r * wavedata->Get(x1, src) + basic.looping * x2r * wavedata->Get(0, src));
                            }
                            else {
                                val = ( x1r * wavedata->Get(x1, src) + x2r * wavedata->Get(x2, src));
                            }
                            
                            data[s*channels+dest] += internal::mastervolume * basic.volume * internal::volume[dest] * val;
                            
                            basic.position += secpersample;
                        }
                    };
                    
                    for(unsigned c = 0; c<wavedata->GetChannelCount(); c++) {
                        auto channel = wavedata->GetChannelType(c);

                        if(channel == Channel::Mono) { //* Mono is distributed to all channels
                            distributetoall(c);
                        }
                        else {
                            int ind = Current.FindChannel(wavedata->GetChannelType(c));
                            
                            if(channel == Channel::FrontLeft) {
                                if(ind!=-1) sendto(c, ind);
                                
                                ind = Current.FindChannel(Channel::BackLeft);
                                
                                if(ind!=-1 && !wavedata->FindChannel(Channel::BackLeft)) sendto(c, ind);
                            }                            
                            else if(channel == Channel::FrontRight) {
                                if(ind!=-1) sendto(c, ind);
                                
                                ind = Current.FindChannel(Channel::BackRight);
                                
                                if(ind!=-1 && !wavedata->FindChannel(Channel::BackRight)) sendto(c, ind);
                            }
                            else if(channel == Channel::Center) {
                                if(ind!=-1) {
                                    sendto(c, ind);
                                }
                                else {
                                    ind = Current.FindChannel(Channel::FrontLeft);
                                    
                                    if(ind!=-1) sendto(c, ind);
                                    
                                    ind = Current.FindChannel(Channel::FrontRight);
                                    
                                    if(ind!=-1) sendto(c, ind);
                                }
                            }
                            else if(channel == Channel::BackLeft) {
                                if(ind!=-1) {
                                    sendto(c, ind);
                                }
                                else {
                                    ind = Current.FindChannel(Channel::FrontLeft);
                                    
                                    if(ind!=-1) sendto(c, ind);
                                }
                            }
                            else if(channel == Channel::BackRight) {
                                if(ind!=-1) {
                                    sendto(c, ind);
                                }
                                else {
                                    ind = Current.FindChannel(Channel::FrontRight);
                                    
                                    if(ind!=-1) sendto(c, ind);
                                }
                            }
                            else if(channel == Channel::LowFreq) {
                                if(ind!=-1) {
                                    sendto(c, ind);
                                }
                                else {
                                    distributetoall(c);
                                }
                            }
                            else {
                                Log << "Unknown channel type: " << String::From(channel);
                            }
                        }
                    }
				}
			}
			
			internal::ControllerMtx.unlock();

			
// 			for(int i=0; i<size; i++) {
// 				test<<data[i*channels]<<"\n";
// 			}
			//std::cout<<"Done.."<<std::endl;
			PostData(data, size*channels);
			std::this_thread::yield();
		}
	}
} }
