#pragma once

#include "../Utils/Any.h"
#include "../Utils/EventChain.h"
#ifdef _DEBUG
#include <assert.h>
#endif

///-This file contains animation support classes and
/// functions

namespace gge {
	class GGEMain;

	////This function should be called before any animation related
	/// function is called.
	void InitializeAnimation(GGEMain *main);

	////This is the base class for all animations.
	/// Provides several ready functions.
	class AnimatorBase {
	public:
		AnimatorBase();
		~AnimatorBase();
		////This event is fired when the animation
		/// completes
		EventChain<AnimatorBase, empty_event_params> FinishedEvent;
		////This event is fired whenever the animation
		/// is interrupted when it reaches the time
		/// specified by PauseAt function
		EventChain<AnimatorBase, empty_event_params> InterruptedEvent;

		////Starts or continues animation
		virtual void Play();
		////Pauses the animation
		virtual void Pause();
		////Progresses animation
		virtual void Progress();
		////Progresses the animation by the given time,
		/// this function is also effected by speed
		virtual void ProgressBy(int ms);
		////Sets a pause position at the given time.
		/// When animation reaches or passes that time
		/// animation is paused and interrupt event
		/// is fired. Negative values cancel this function.
		void PauseAt(int ms);
		////Cancels the set pause position
		void CancelPauseAt() { PauseAt(-1); }
		////Instantly progresses to the given time. Notice that
		/// some animations cannot handle going back in time.
		/// This function is not affected by speed
		virtual void ProgressTo(float ms);
		////Changes the speed of the animation, some animation may
		/// dislike changing this value
		///@Speed		: speed of the animation, default is 1 while
		/// 0 effectively stops the animation
		virtual void setSpeed(float Speed);
		////Returns the current time of the animation
		int currentTime();
		////Returns whether the animation is playing
		bool isPlaying();

	protected:
		////This function is called whenever processing
		/// is required. Animator object can use
		/// progressed variable to determine the current
		/// relative time.
		virtual void Process(int time)=0;

		////Returns true when animation completes.
		/// This function should be implemented by
		/// animator class.
		virtual bool isFinished()=0;

		////The time of the last tick, used to determine
		/// process time
		unsigned int lasttick;
		////Used for pause support
		unsigned int pausedat;
		////Time progressed since the beginning of the
		/// animation, double is used to handle slower
		/// speeds
		float progressed;
		////Whether the animation is paused
		bool ispaused;
		float speed;
		////Used for PauseAt support
		int pauseat;
	};

	////Used pass current and previous frame numbers when a frame change
	/// event occurs
	struct frame_changed_event_params {
		frame_changed_event_params() { }
		frame_changed_event_params(int prev,int cur) {
			previous=prev;
			current=cur;
		}
		////Previous frame
		int previous;
		////Current frame
		int current;
	};

	////This base class defines a discreate animator,
	/// it includes few common variables and functions
	class DiscreteAnimatorBase : public AnimatorBase {
	public:
		////Constructor to setup the system
		DiscreteAnimatorBase();
		////Fired whenever a frame change occurs when EventOnFrameChange
		/// flag is set
		EventChain<DiscreteAnimatorBase, frame_changed_event_params> FrameChangedEvent;
		////This event is fired whenever the animation
		/// is interrupted when it reaches the frame
		/// specified by PauseAtFrame function
		EventChain<DiscreteAnimatorBase, empty_event_params> FrameInterruptedEvent;
		////This function sets the current frame to the given value.
		/// Animation time is moved to the beginning of the given frame
		///@Frame		: the frame to go
		virtual void GotoFrame(int Frame);
		////Sets the whether to loop or not.
		virtual void setLoop(bool loop);
		////Sets a pause position at the given frame.
		/// When animation reaches or passes that frame
		/// animation is paused and frameinterrupt event
		/// is fired. Negative values cancel this function.
		virtual void PauseAtFrame(int Frame);
		////Cancels the set pause position
		void CancelPauseAtFrame() { PauseAtFrame(-1); }
		////Reverses the direction of the animation
		virtual void Reverse();
		////Sets animation direction to forwards
		virtual void GoForwards();
		////Sets animation direction to backwards
		virtual void GoBackwards();
		////Returns whether the animation is looping
		virtual bool isLooping();
		////Changes the speed of the animation, some animation may
		/// dislike changing this value
		///@Speed		: speed of the animation, default is 1 while
		/// 0 effectively stops the animation
		virtual void setSpeed(float Speed) {
			if(gobackwards)
				AnimatorBase::setSpeed(-Speed);
			else
				AnimatorBase::setSpeed( Speed);
		}
		////Returns whether the animation is playing
		/// backwards
		virtual bool isBackwards();
		////Returns the current frame
		virtual int currentFrame();
		////Returns total frames in this animation
		int totalFrames();
		////Overrides play to roll animation to beginning
		virtual void Play() { 
			if(gobackwards && progressed==0) progressed=(float)duration-1; 
			else if(!gobackwards && progressed==duration-1) duration=0;
			AnimatorBase::Play();
		}

		////If this flag is set, an event is fired when a frame
		/// transition occurs
		bool EventOnFrameChange;
	protected:
		////This value should not be modified by animator,
		/// instead setTotalFrames function should be used
		int totalframes;
		////This value is used to internally and automatically
		/// calculated and should not be changed. The value
		/// is probed when setTotalFrames is called
		int duration;
		////Whether the animation is looping state
		bool islooping;
		////Whether the animation is running backwards
		bool gobackwards;
		////This value should not be modified by animator,
		/// instead setTotalFrames function should be used
		int *frametimings;
		////This value should not be modified by animator,
		/// instead setTotalFrames function should be used
		int *framedurations;
		////The current frame, should not be changed by animator,
		/// GotoFrame function should be used in place
		int currentframe;
		////This value is used to speed up searches,
		/// should only be changed when necessary. The value
		/// is probed when setTotalFrames is called
		int averagedurations;
		////Used for pause@frame support
		int pauseatframe;


		////Used to change the total number of frames
		/// by the animator, invokes probing of frame timings
		/// and durations
		void setTotalFrames(int total);
		////This function should return the duration
		/// time of frames
		virtual int* FrameDurations()=0;
		////Processes the current time to determine current frame
		virtual void Process(int time);
		////Can be overriden to process a frame,
		/// a default method doing nothing is provided
		virtual void ProcessFrame(int frame);
		////This function returns whether this animation is finished
		virtual bool isFinished() { return !islooping && ( (currentframe==totalframes-1 && !gobackwards) || (currentframe==0 && gobackwards) ); }
	};
}
