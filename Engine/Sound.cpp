#include "Sound.h"
#include "GGEMain.h"

namespace gge { namespace sound {

	namespace system {
		bool isAvailable=false;

		int InitializeSound(os::WindowHandle Window, char *device) {
			isAvailable=false;

			///*Opening audio device
			ALCdevice *Device=alcOpenDevice(device);
			if(Device==NULL)
				return ERRAL_DEVICE_ERR;

			///*Creating device context
			ALCcontext *context=alcCreateContext(Device,NULL);
			if(context==NULL)
				return ERRAL_CONTEXT_ERR;

			alcMakeContextCurrent(context);

			isAvailable=true;

			environment::SetListener3DPosition(Main.getWidth()/2.f,Main.getHeight()/2.f,0);

			return ERRNO_ERR;
		}

		SoundBufferHandle CreateSoundBuffer(WaveFormat Format, void *Data, int Size) {
			SoundBufferHandle buffer=0;

			///*Creating Buffer
			alGetError();
			alGenBuffers(1,&buffer);
			if(alGetError()!=AL_NO_ERROR)
				return 0;

			///*Deciding OpenAL wave format
			ALenum format;
			if(Format.Channels==1) {
				if(Format.BitsPerSample==8) {
					format=AL_FORMAT_MONO8;
				} else {
					format=AL_FORMAT_MONO16;
				}
			} else {
				if(Format.BitsPerSample==8) {
					format=AL_FORMAT_STEREO8;
				} else {
					format=AL_FORMAT_STEREO16;
				}
			}

			///*Filling buffer
			alBufferData(buffer, format, Data, Size, Format.SamplesPerSec);

			return buffer;
		}

		SoundControlHandle CreateSoundController(SoundBufferHandle Buffer) {
			SoundControlHandle source;

			alGetError();
			alGenSources(1,&source);
			int ret=alGetError();
			if(ret==AL_NO_ERROR)
				alSourceQueueBuffers(source, 1, &Buffer);

			alSourcef(source, AL_ROLLOFF_FACTOR, 0);

			return source;
		}

		gge::sound::system::SoundControlHandle Create3DSoundController( SoundBufferHandle Buffer, float maxWaveDistance )
		{
			SoundControlHandle source;

			alGetError();
			alGenSources(1,&source);
			int ret=alGetError();
			if(ret==AL_NO_ERROR)
				alSourceQueueBuffers(source, 1, &Buffer);

			alSourcef(source, AL_ROLLOFF_FACTOR, 1.0);
			alSourcef(source, AL_REFERENCE_DISTANCE, maxWaveDistance);

			return source;
		}	

		void DestroySoundBuffer(SoundBufferHandle &handle) {
			alDeleteBuffers(1, &handle);
		}

	}

	namespace environment {
		void SetListener3DPosition(float x,float y,float z) {
			alListener3f(AL_POSITION,x,y,z);
		}
	}
} }
