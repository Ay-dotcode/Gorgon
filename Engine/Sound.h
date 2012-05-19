#pragma once

#include "GGE.h"
#include "OS.h"
#include "../External/OpenAL/alc.h"
#include "../External/OpenAL/al.h"

#define ERRNO_ERR			0
#define ERRAL_DEVICE_ERR	1
#define ERRAL_CONTEXT_ERR	2

namespace gge { namespace sound {
	namespace system {
		////Handle type for sound buffers
		typedef ALuint	SoundBufferHandle;
		////Handle type for sound controllers
		typedef ALuint	SoundControlHandle;

		////Wave format definition, compatible with
		/// Win32 MMSystem's WaveFormatEx structure
		struct WaveFormat {
			////Format type
			WORD        FormatTag;
			////Number of channels, mono and sterio are fully 
			/// supported, rest is untested
			WORD        Channels;
			////Samples/second (like 22000 for 22kHz)
			DWORD       SamplesPerSec;
			////Avarage bytes/second can be used as statistics
			DWORD       AvgBytesPerSec;
			///*???
			WORD        BlockAlign;
			////Bits/Sample, represents the quality when combined with Samples/second
			/// this data is for single channel
			WORD        BitsPerSample;
			////Lame and useless, the size of the structure,
			/// retained for compatibility
			WORD        Size;
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
		///@Format	: Format of given buffer, only mono, streo and 8bit, 16bit
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