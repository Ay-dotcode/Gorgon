/// @file contains animation system

#pragma once

#include <stdexcept>

#include "Event.h"
#include "Containers/Collection.h"

namespace Gorgon { 
	/// This namespace contains animation related functionality. 
	namespace Animation {

		class Base;

		/// Timers are required to progress animations. This class is the most basic timer
		/// and does not support any operations. It linearly progresses animation and never
		/// stops. Most animations are expected loop under these circumstances. See Controller 
		/// for additional functionality.
		class Timer {
		public:

			/// Constructs a timer
			Timer();

			/// Timer destructor
			virtual ~Timer();

			/// Progresses this timer by moving the timer timepassed milliseconds forwards
			virtual void Progress(unsigned timepassed);

			/// This function notifies the timer about a finished animation. Base timer does
			/// not perform any operation when an animation attached to it is finished.
			/// @param  leftover is the amount of time that is left over after the animation
			///         is progress to the end
			virtual void Finished(unsigned leftover) { }

			/// Resets the timer, basically starting the animation from the start.
			virtual void Reset() { 
				progress=0; 
			}

			/// Sets the progress of the timer to the given value.
			virtual void SetProgress(unsigned progress) { 
				this->progress=progress; 
			}

			/// Returns the current progress of the timer
			virtual unsigned GetProgress() const { 
				return progress; 
			}

			/// This method allows clients to determine if the progress is controlled. If the progress
			/// is not controlled, there is no way to force the animation to stop. Therefore, animations
			/// with looping capabilities should wrap around to start over. However, if the timer is a
			/// controller then the best strategy will be to stop at the end, and return the leftover time.
			/// This way, controller can decide what to do next.
			virtual bool IsControlled() const {
				return false;
			}

		protected:
			/// Amount of time passed since the start of the animation
			unsigned progress = 0;
		};

		class Controller : public Timer {
		public:

			Controller();

			virtual ~Controller() {}

			/// @name Progress modification functions
			/// @{

			/// Progresses this controller by the given time
			virtual void Progress(unsigned timepassed);

			/// Signals that an animation bound to this controller is finished.
			/// @param  leftover is the time that is left after the animation is completely finished
			virtual void Finished(unsigned leftover);

			/// Sets the current progress of the controller
			virtual void SetProgress(unsigned progress) { floatprogress=this->progress=progress; }

			/// Sets the current progress of the controller. If the progress is a negative value, it will be
			/// subtracted from the animation length. If the animation length is 0, then the controller will
			/// immediately stop and sets the progress to 0.
			void SetProgress(double progress) { this->progress=(unsigned)std::round(progress); floatprogress=progress; }

			/// Resets the controller to start from the beginning. Also resets finished and paused status and
			/// modifies the speed to be 1.
			virtual void Reset();
			/// @}

			/// @name Progress control functions
			/// @{

			/// Starts this controller to run once. If the controller is marked as finished, this method will 
			/// set the progress to 0 or length depending on the direction of the controller. If length is 0 
			/// and the speed is negative this method will not start playing finished controller. If the animation
			/// is paused, this function works like Resume except that this function sets controller to run once mode.
			virtual void Play();

			/// Starts this controller in looping mode. Looping will not work when the length is 0 and the speed 
			/// is set to a negative value (animation is running in reverse). If the animation is paused, this 
			/// function works like Resume except that this function sets controller to looping mode
			virtual void Loop() { Play(); islooping=true; }

			/// Pauses the controller, until a Resume or Reset is issued.
			virtual void Pause();

			/// Resumes the controller. This method will not have any effect if the animation is finished.
			virtual void Resume() { ispaused=false; }

			/// Changes the speed of the controller. Speed can be negative to run animations backwards. Setting
			/// speed 0 effectively pauses the controller, however, when the speed is 0 controller will **not** report
			/// that its paused.
			virtual void SetSpeed(float speed) { this->speed=speed; }

			/// Reverses the animation direction by negating the speed.
			virtual void Reverse() { speed=-speed; }

			/// Informs controller about the length of the animations its controlling. This allows Controller to seek
			/// to the end of the animation
			virtual void SetLength(unsigned length) { this->length=length; }
			/// @}

			/// @name Information functions
			/// @{

			/// Returns the current speed of the controller
			virtual float GetSpeed() { return speed; }

			/// Returns whether the controller is paused. Does not check if the speed is 0 or not, setting speed to 0 
			/// will effectively pause the animation without changing paused status.
			bool IsPaused() const { return ispaused; }

			/// Whether the controller is finished either by reaching to the end while the speed is positive or reaching to
			/// 0 while the speed is negative
			bool IsFinished() const { return isfinished; }

			/// Returns whether the controller is playing its animations right now. This method does not take speed being
			/// 0 into account.
			bool IsPlaying() const { return !ispaused && !isfinished; }

			/// Checks whether the controller is in loop mode. It also checks the length if the speed is negative and makes
			/// sure that the controller can actually loop
			bool IsLooping() const { return (speed>=0 ? islooping : islooping && length!=0); }

			virtual bool IsControlled() const { return true; }
			/// @}

			/// @name Events
			/// @{

			/// Will be fired when the controller reaches the finished state. controller is finished either by reaching to 
			/// the end while the speed is positive or reaching to 0 while the speed is negative
			Event<Controller> FinishedEvent;
			/// @}

		protected:
			/// Paused state
			bool ispaused = false;

			/// Looping state
			bool islooping = false;

			/// Current speed
			float speed = 1.0f;

			/// Whether the controller is finished
			bool isfinished = false;

			/// Floating point progress to avoid precision loss due to speed
			double floatprogress = 0.0;

			/// Length of the animations controlled by this controller
			unsigned length=0;
		};

		/// This interface marks a class as animation provider
		class Provider {
		public:
			/// Virtual destructor
			virtual ~Provider() { }
		
			/// This function should create a new animation with the given controller and
			/// if owner parameter is set to true, it should assume ownership of the controller
			virtual Base &CreateAnimation(Timer &timer, bool owner=false) = 0;

			/// This function should create and animation and depending on the create parameter,
			/// it should create its own timer.
			virtual Base &CreateAnimation(bool create=false) = 0;
		};

		/// This is the base class for all animations. It handles some common tasks and defines
		/// the animation interface.
		class Base {
		public:
			/// This constructor takes a controller and depending on the owner parameter assumes the ownership
			/// of it. Be careful not to give the ownership of a stack allocated timer.
			Base(Timer &timer, bool owner=false);

			/// This constructor creates a new controller depending on the create parameter. 
			explicit Base(bool create=false);
		
			/// Virtual destructor
			virtual ~Base();

			/// Sets the controller to the given controller. If owner parameter is true, this object
			/// will assume the ownership of that controller. Current controller of this animation will
			/// be destroyed if this animation already has a controller and has the ownership over it.
			/// You may use disown to remove ownership of the controller.
			virtual void SetController(Timer &controller, bool owner=false);

			/// Returns whether this animation has a controller
			bool HasController() const { return controller!=nullptr; }

			/// Returns the controller of this animation
			Timer &GetController() const { return *controller; }

			/// Disowns the controller that this animation has
			void DisownController() { owner = false; }

			/// Removes the controller of this animation. Current controller of this animation will
			/// be destroyed if this animation already has a controller and has the ownership over it.
			/// You may use disown to remove ownership of the controller.
			void RemoveController() { 
				if(owner && controller) {
					delete controller;
				}
				controller=nullptr; 
			}

			/// This function should progress the animation. Notice that this function is called internally.
			/// Unless a change to the controller has been made and instant update of the animation is required
			/// there is no need to call this function. Returning true from this function denotes that further
			/// progress is possible. If progress should end, leftover parameter should be set to the amount of
			/// time that cannot be progressed.
			virtual bool Progress(unsigned &leftover) = 0;

			/// Deletes this animation. Please note that some animations are also the animation provider. In these
			/// cases trying to delete the animation will delete the provider as well. This function should be called
			/// instead of delete operator to ensure no such problem occurs.
			virtual void DeleteAnimation() { 
				delete this; 
			}

		protected:
			/// The controller of this animation
			Timer *controller = nullptr;

			/// Whether this animation owns its controller
			bool owner = false;
		};

		extern Containers::Collection<Base> Animations;
	} 
}
