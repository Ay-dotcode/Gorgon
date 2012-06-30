#pragma once

#include "GGE.h"
#include "OS.h"


namespace gge { namespace sound {
	namespace system {
		static const int ERRNO_ERR = 0;
		static const int ERRAL_DEVICE_ERR=1;
		static const int ERRAL_CONTEXT_ERR=2;
		////Handle type for sound buffers
		typedef unsigned int SoundBufferHandle;
		////Handle type for sound controllers
		typedef unsigned int SoundControlHandle;

		////Wave format definition, compatible with
		/// Win32 MMSystem's WaveFormatEx structure
		struct WaveFormat {
			////Format type
			unsigned short     FormatTag;
			////Number of channels, mono and stereo are fully 
			/// supported, rest is untested
			unsigned short        Channels;
			////Samples/second (like 22000 for 22kHz)
			unsigned int       SamplesPerSec;
			////Average bytes/second can be used as statistics
			unsigned int       AvgBytesPerSec;
			///*???
			unsigned short        BlockAlign;
			////Bits/Sample, represents the quality when combined with Samples/second
			/// this data is for single channel
			unsigned short        BitsPerSample;
		};


		////Initializes the sound sub system, if no sound
	/// can be used this function fails gracefully disabling
	/// any audio attempt without causing any trouble.
	/// soundavailable variable can be checked whether 
	/// sound system is up and running.
	///@Window	: Handle of the main window 
	///@Device	: The device identifier to be created, NULL will
	/// use default device.
		int InitializeSound(os::WindowHandle Window, char *Device=NULL);

		////Creates a new wave from a given buffer.
		/// Returns sound handle that can be used to run
		/// the created buffer
		///@Format	: Format of given buffer, only mono, stereo and 8bit, 16bit
		/// types work
		///@Data	: Pointer to the wave data
		///@Size	: The size of the wave data
		SoundBufferHandle CreateSoundBuffer(WaveFormat Format, void *Data, int Size);

		void DestroySoundBuffer(SoundBufferHandle &handle);

		////Creates a new sound controller to play a given buffer
		SoundControlHandle CreateSoundController(SoundBufferHandle Buffer);

		////Creates a new sound controller with 3D positional properties. maxWaveDistance
		/// allows the control of attenuation
		SoundControlHandle Create3DSoundController(SoundBufferHandle Buffer, float maxWaveDistance);

		////Used to identify if sound is available
		extern bool	isAvailable;
	}

	namespace environment {
		////Changes the position of the sound listener for 3D positional elements
		void SetListenerPos(float x,float y,float z);

		////Changes Position of 3D Listener
		void SetListener3DPosition(float x,float y,float z);
	}
} }