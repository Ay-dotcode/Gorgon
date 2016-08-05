#include <cstring>
#include "Environment.h"

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
		
		float BufferDuration = 0.030f; //in seconds
		int   BufferSize     = 0; //if left 0, filled by audio loop
		
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

		int   freq 		   = Current.GetSampleRate();
		float secpersample = 1.0f / freq;
		int channels       = Current.GetChannelCount();
        
        Environment::Current.init();

        if(internal::BufferSize == 0)
            internal::BufferSize = int(freq * internal::BufferDuration);

		int datasize = channels * internal::BufferSize;
		
		std::vector<float> data(datasize*sizeof(float));
        
        std::vector<float> temp(internal::BufferSize);

		//std::ofstream test("test.csv");
		
		while(true) {
			int maxsize, size;

			maxsize = GetWritableSize(channels);
			size    = maxsize;
			size    = std::min(size, internal::BufferSize);

			if(!size) {
				SkipFrame();
				std::this_thread::yield();
				continue;
			}
			
			//zero out before starting
			std::memset(&data[0], 0, sizeof(float) * size * channels);
			
			internal::ControllerMtx.lock();
					
            auto outofbounds = [](BasicController &basic) {
                //reached to the end of the stream
                if(basic.looping) {
                    basic.position = basic.position - basic.GetDuration();
                    
                    return true;
                }
                else {
                    basic.position = basic.GetDuration();
                    basic.playing = false;
                    
                    return false;
                }
            };
            
            auto interpolate_basic = [&outofbounds](BasicController &basic, int ch) {
                
                auto wavedata = basic.wavedata;
                
            recalculate:
                float pos     = basic.position * wavedata->GetSampleRate();
                
                int x1 = (int)pos      ;
                int x2 = (int)(pos + 1);
                
                if(x1 >= (int)wavedata->GetSize()) {
                    if(outofbounds(basic)) goto recalculate;
                    else return 0.f;
                }
                
                float x1r = x2 - pos;
                float x2r = 1  - x1r;
                
                float val;
                
                if(x2 >= (int)wavedata->GetSize()) {
                    // multiplication with basic.looping is for branch elimination. Basically, if the audio is not looping
                    // last few values will not be interpolated but faded out.
                    val = ( x1r * wavedata->Get(x1, ch) + basic.looping * x2r * wavedata->Get(0, ch));
                }
                else {
                    val = ( x1r * wavedata->Get(x1, ch) + x2r * wavedata->Get(x2, ch));
                }
                
                return val;
           };
            
            //optimization might be necessary, this function may cause cache misses for many to many mapping
            auto sendto = [&](BasicController &basic, float org, int src, int dest, float vol = 1.f) {
                basic.position = org;
                
                for(int s=0; s<size; s++) {
                    data[s*channels+dest] += vol * internal::mastervolume * basic.volume * internal::volume[dest] * interpolate_basic(basic, src);
                    
                    if(basic.position == basic.GetDuration()) break;
                    
                    basic.position += secpersample;
                }
            };
			
			//For cache optimization we would do vertical sampling
			for(auto &controller : internal::Controllers) {
				if(controller.Type() == ControllerType::Basic) {
					auto &basic = static_cast<BasicController&>(controller);
					
					//std::cout<<"Playing.."<<std::endl;
					
					if(!basic.playing) continue;
					
					if(!basic.wavedata) continue;
					
					auto wavedata = basic.wavedata;
					
                    auto org = basic.position;
                    
                    auto distributetoall = [&](int ind) {
                        basic.position = org;
                        
                        for(int s=0; s<size; s++) {                            
                            float val = internal::mastervolume * basic.volume * interpolate_basic(basic, ind);
                    
                            if(basic.position == basic.GetDuration()) break;
                            
                            for(int c = 0; c<channels; c++) {
                                data[s*channels+c] += internal::volume[c] * val;
                            }
                            
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
                                if(ind!=-1) sendto(basic, org, c, ind);
                                
                                ind = Current.FindChannel(Channel::BackLeft);
                                
                                if(ind!=-1 && !wavedata->FindChannel(Channel::BackLeft)) sendto(basic, org, c, ind);
                            }                            
                            else if(channel == Channel::FrontRight) {
                                if(ind!=-1) sendto(basic, org, c, ind);
                                
                                ind = Current.FindChannel(Channel::BackRight);
                                
                                if(ind!=-1 && !wavedata->FindChannel(Channel::BackRight)) sendto(basic, org, c, ind);
                            }
                            else if(channel == Channel::Center) {
                                if(ind!=-1) {
                                    sendto(basic, org, c, ind);
                                }
                                else {
                                    ind = Current.FindChannel(Channel::FrontLeft);
                                    
                                    if(ind!=-1) sendto(basic, org, c, ind);
                                    
                                    ind = Current.FindChannel(Channel::FrontRight);
                                    
                                    if(ind!=-1) sendto(basic, org, c, ind);
                                }
                            }
                            else if(channel == Channel::BackLeft) {
                                if(ind!=-1) {
                                    sendto(basic, org, c, ind);
                                }
                                else {
                                    ind = Current.FindChannel(Channel::FrontLeft);
                                    
                                    if(ind!=-1) sendto(basic, org, c, ind);
                                }
                            }
                            else if(channel == Channel::BackRight) {
                                if(ind!=-1) {
                                    sendto(basic, org, c, ind);
                                }
                                else {
                                    ind = Current.FindChannel(Channel::FrontRight);
                                    
                                    if(ind!=-1) sendto(basic, org, c, ind);
                                }
                            }
                            else if(channel == Channel::LowFreq) {
                                if(ind!=-1) {
                                    sendto(basic, org, c, ind);
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
				else if(controller.Type() == ControllerType::Positional) {
                    auto &positional = static_cast<PositionalController&>(controller);
					
					if(!positional.playing) continue;
					
					if(!positional.wavedata) continue;
					
					auto wavedata = positional.wavedata;
					
                    auto org = positional.position;
                    
                    //prefered way
                    if(wavedata->FindChannel(Channel::Mono) != -1) {
                        int ind = wavedata->FindChannel(Channel::Mono);
                        
                        for(int s=0; s<size; s++) {                            
                            float val = internal::mastervolume * positional.volume * interpolate_basic(positional, ind);
                            
                            temp[s] = val;
                            
                            positional.position += secpersample;
                       }
                    }
                    else {
                        for(int s=0; s<size; s++) {
                            float val = 0;
                            
                            for(int c=0; c<(int)wavedata->GetChannelCount(); c++) {
                                if(wavedata->GetChannelType(c) != Channel::LowFreq) {
                                    val += internal::mastervolume * positional.volume * interpolate_basic(positional, c);
                                }
                            }
                            
                            temp[s] = val;
                            
                            positional.position += secpersample;
                       }
                    }
                    
                    if(wavedata->FindChannel(Channel::LowFreq) != -1) {
                        if(Current.FindChannel(Channel::LowFreq) != -1) {
                            sendto(positional, org, wavedata->FindChannel(Channel::LowFreq), Current.FindChannel(Channel::LowFreq), 
                                   std::exp(-Environment::Current.attuniationfactor * (positional.location - Environment::Current.listener.location).Distance()));
                        }
                        else {
                            int ind = wavedata->FindChannel(Channel::LowFreq);
                            positional.position = org;
                            
                            for(int s=0; s<size; s++) {                            
                                float val = internal::mastervolume * positional.volume * interpolate_basic(positional, ind);
                                
                                temp[s] = val;
                                
                                positional.position += secpersample;
                            }
                        }
                    }
                    
                    
                    //headphones
                    if(Current.IsHeadphones()) {                        
                        auto &env = Environment::Current;
                        auto &lis = env.listener;
                        
                        float leftvol, rightvol;
                        
                        auto loc  = positional.location;
                        
                        auto leftvec  = (loc - lis.leftpos);
                        auto rightvec = (loc - lis.rightpos);
                        
                        leftvol = leftvec.Normalize() * env.left;
                        leftvol += 1;
                        leftvol /= 2;
                        //leftvol *= leftvol;
                        
                        rightvol = rightvec.Normalize() * env.right;
                        rightvol += 1;
                        rightvol /= 2;
                        //rightvol *= rightvol;
                        
                        auto total = (leftvol + rightvol);
                        
                        leftvol  /= total;
                        rightvol /= total;

                        auto mult = std::exp(-env.attuniationfactor * leftvec.Distance());

                        leftvol = (leftvol * (1 - Environment::Current.nonblocked) + Environment::Current.nonblocked) * mult;
                        
                        mult = std::exp(-env.attuniationfactor * rightvec.Distance());

                        rightvol = (rightvol * (1 - Environment::Current.nonblocked) + Environment::Current.nonblocked) * mult;
                        
                        int leftind  = Current.FindChannel(Channel::FrontLeft);
                        int rightind = Current.FindChannel(Channel::FrontRight);
                        
                        //std::cout<<leftvol<< " : " <<rightvol<<std::endl;
                        
                        for(int s=0; s<size; s++) {
                            data[s*channels+leftind]  +=  leftvol * temp[s];
                            data[s*channels+rightind] += rightvol * temp[s];
                        }
                    }
                    
                    //stereo
                    else if(Current.FindChannel(Channel::BackLeft) == -1) {                        
                        auto &env = Environment::Current;
                        auto &lis = env.listener;
                        
                        float leftvol, rightvol;
                        
                        auto diff = positional.location - lis.location;;
                        auto w = diff.Normalize();
                        
                        leftvol  = w * env.speaker_vectors[0];                        
                        rightvol = w * env.speaker_vectors[1];
                        
                        leftvol += 1;
                        leftvol /= 2;
                        
                        rightvol += 1;
                        rightvol /= 2;
                        
                        auto total = leftvol + rightvol;
                        leftvol /= total;
                        rightvol /= total;
                        
                        leftvol  *= std::exp(-env.attuniationfactor * (diff.Distance()-env.speaker_boost[0]));
                        rightvol *= std::exp(-env.attuniationfactor * (diff.Distance()-env.speaker_boost[1]));
                         
                        int leftind  = Current.FindChannel(Channel::FrontLeft);
                        int rightind = Current.FindChannel(Channel::FrontRight);
                       
                        //std::cout<<leftvol<< " : " <<rightvol<<std::endl;
                        
                        for(int s=0; s<size; s++) {
                            data[s*channels+leftind]  +=  leftvol * temp[s];
                            data[s*channels+rightind] += rightvol * temp[s];
                        }
                    }
                    
                    //surround
                    else {                        
                        auto &env = Environment::Current;
                        auto &lis = env.listener;
                        
                        float fl, fr, bl, br;
                        
                        auto diff = positional.location - lis.location;;
                        auto w = diff.Normalize();
                        
                        fl = w * env.speaker_vectors[0];                        
                        fr = w * env.speaker_vectors[1];                     
                        bl = w * env.speaker_vectors[2];                     
                        br = w * env.speaker_vectors[3];
                        
                        if(fl<0) fl = 0;
                        
                        if(fr<0) fr = 0;
                       
                        if(bl<0) bl = 0;
                        
                        if(br<0) br = 0;
                        
                        auto total = fl + fr + bl + br;
                        fl /= total;
                        fr /= total;
                        bl /= total;
                        br /= total;
                        
                        fl *= std::exp(-env.attuniationfactor * (diff.Distance()-env.speaker_boost[0]));
                        fr *= std::exp(-env.attuniationfactor * (diff.Distance()-env.speaker_boost[1]));
                       
                        bl *= std::exp(-env.attuniationfactor * (diff.Distance()-env.speaker_boost[2]));
                        br *= std::exp(-env.attuniationfactor * (diff.Distance()-env.speaker_boost[3]));
                         
                        int flind = Current.FindChannel(Channel::FrontLeft);
                        int frind = Current.FindChannel(Channel::FrontRight);
                         
                        int blind = Current.FindChannel(Channel::BackLeft);
                        int brind = Current.FindChannel(Channel::BackRight);
                       
                        //std::cout<<fl<< " : " <<fr<<" | "<<bl<< " : " <<br<<std::endl;
                        
                        for(int s=0; s<size; s++) {
                            data[s*channels+flind]  +=  fl * temp[s];
                            data[s*channels+frind]  +=  fr * temp[s];
                            data[s*channels+blind]  +=  bl * temp[s];
                            data[s*channels+brind]  +=  br * temp[s];
                        }
                    }
                } //controller type
                
			}
			
			internal::ControllerMtx.unlock();

			
// 			for(int i=0; i<size; i++) {
// 				test<<data[i*channels]<<"\n";
// 			}
			//std::cout<<"Done.."<<std::endl;
			PostData(&data[0], size, channels);
			std::this_thread::yield();
		}
	}
	
	Environment Environment::Current;
} }
