#pragma once

#include "GGEMain.h"
#include "Sound.h"
#include "../Utils/Collection.h"

namespace gge { namespace sound {

	class Wave;

	namespace system {
		////This function initializes garbage collection subsystem
		void InitWaveGarbageCollect(GGEMain *gge);

		////This function checks finished waves that are marked as
		/// garbage collectable and destroys them.
		void CollectWaveGarbage();
	}

	////This is function definition required to signal
	/// when a wave finishes its execution
	typedef void(*wavefinish)(Wave &wave);



	////Class to control wave-type sounds, this class handles
	/// the creation of controller and supports 3D positioning
	/// looping sounds, auto destruction, volume adjustment
	/// and an event handler to be run when finished
	class Wave {
	public:
		////default constructor requires buffer handle.
		///@maxWaveDistance	: if specified the generated sound controller
		/// will have 3D properties
		Wave(system::SoundBufferHandle Buffer, int maxWaveDistance=0);
		////Plays this sound once
		Wave& Play();
		////Plays this sound continously until stopped
		Wave& Loop();
		////Stops the sound
		Wave& Stop();
		////Used to adjust the volume, it can decrease or increase the
		/// output, 1.0 is volume at 100% while 0.25 is at 25%
		Wave& SetVolume(float Volume);
		////Adjusts 3D position of the current sound source allowing
		/// attuniation to take place.
		Wave& Set3DPosition(float x,float y,float z);

		////Used to check if sound is being played
		bool isPlaying();

		////Whether this sound should free itself when it is finished
		bool AutoDestruct;

		////This event is fired when sound buffer is played.
		/// This event is fired by GGEMain while sound is being
		/// examined for auto destruction.
		wavefinish finished;

		////Default destroyer used to free resources
		~Wave();
		////Destroys the controller that this class is bound to
		void _destroy();

	protected:
		////Whether finished event is fired or not
		bool finishedstateisknown;
		////The wave buffer
		system::SoundBufferHandle	buffer;
		////The controller
		system::SoundControlHandle	source;
		////Whether this sound is available to be played
		bool isavailable;
	};


} }
