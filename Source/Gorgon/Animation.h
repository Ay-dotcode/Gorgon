/// @file 
/// contains animation system

#pragma once

#include <stdexcept>
#include <iostream>

#include "Event.h"
#include "Containers/Collection.h"

namespace Gorgon { 
	/// This namespace contains animation related functionality. 
	namespace Animation {

		class Base;

		/// Controllers are required to progress animations
		class ControllerBase {
		public:
			/// Default constructor
			ControllerBase();

			/// Destructor
			virtual ~ControllerBase();

			/// Progresses this timer by moving the timer timepassed milliseconds forwards
			virtual void Progress(unsigned timepassed)	= 0;

			/// This function attaches the given animation to this controller
			virtual void Add(Base &animation);

			/// Removes the given animation
			virtual void Remove(Base &animation);

			/// Deletes the given animation
			virtual void Delete(Base &animation);

			/// Returns the current progress of the timer
			virtual unsigned GetProgress() const        = 0;

			/// This method allows clients to determine if the progress is controlled. If the progress
			/// is not controlled, there is no way to force the animation to stop. Therefore, animations
			/// with looping capabilities should wrap around to start over. However, if the timer is a
			/// controller then the best strategy will be to stop at the end, and return the leftover time.
			/// This way, controller can decide what to do next.
			virtual bool IsControlled() const           = 0;

			/// Set a flag that will automatically destroy this controller whenever it has no animations left
			/// to control
			void AutoDestruct() { collectable=true; }

			/// Resets the flag that will automatically destroy this controller whenever it has no animations left
			/// to control
			void Keep() { collectable=false; }
			
			/// Resets the animation to the start. Animation controllers that do not support this request should
			/// silently ignore it.
			virtual void Reset() = 0;

		protected:
			/// Whether this controller should be collected by the garbage collector when its task is finished
			bool collectable = false;

			/// List of animations this controller holds
			Containers::Collection<Base> animations;
		};

		/// This class is the most basic controller and does not support any operations. It linearly progresses 
		/// animation and never stops. Most animations are expected loop under these circumstances. See Controller 
		/// for additional functionality.
		class Timer : public ControllerBase {
		public:

			virtual ~Timer() { }

			/// Progresses this timer by moving the timer timepassed milliseconds forwards
			virtual void Progress(unsigned timepassed) override final;

			/// Resets the timer, basically starting the animation from the start.
			virtual void Reset() override { 
				progress=0; 
			}

			/// Sets the progress of the timer to the given value.
			virtual void SetProgress(unsigned progress) { 
				this->progress=progress; 
			}

			/// Returns the current progress of the timer
			virtual unsigned GetProgress() const override final { 
				return progress; 
			}

			/// This method allows clients to determine if the progress is controlled. If the progress
			/// is not controlled, there is no way to force the animation to stop. Therefore, animations
			/// with looping capabilities should wrap around to start over. However, if the timer is a
			/// controller then the best strategy will be to stop at the end, and return the leftover time.
			/// This way, controller can decide what to do next.
			virtual bool IsControlled() const override final {
				return false;
			}

		protected:
			/// Amount of time passed since the start of the animation
			unsigned progress = 0;
		};

		class Controller : public ControllerBase {
		public:

			/// Default constructor
			Controller(double progress = 0.0);

			virtual ~Controller() {}

			/// @name Progress modification functions
			/// @{

			/// Progresses this controller by the given time
			virtual void Progress(unsigned timepassed) override final;

			/// Sets the current progress of the controller
			virtual void SetProgress(unsigned progress) { this->progress=progress; }

			/// Sets the current progress of the controller. If the progress is a negative value, it will be
			/// subtracted from the animation length. If the animation length is 0, then the controller will
			/// immediately stop and sets the progress to 0.
			void SetProgress(double progress) { this->progress=progress; }

			/// Resets the controller to start from the beginning. Also resets finished and paused status and
			/// modifies the speed to be 1.
			virtual void Reset() override;
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

			virtual bool IsControlled() const override final { return true; }
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
			double progress = 0.0;

			/// Length of the animations controlled by this controller
			unsigned length=0;
		};

		/// This interface marks a class as animation provider
		class Provider {
		public:
			using AnimationType = Base;
            
            /// This function moves this animation provider into a new provider. Ownership of this new object belongs
            /// to the caller and this object could be destroyed safely.
            virtual Provider &MoveOutProvider() = 0;

			/// Virtual destructor
			virtual ~Provider() { }
		
			/// This function should create a new animation with the given controller
			virtual Base &CreateAnimation(ControllerBase &timer) const = 0;

			/// This function should create an animation and depending on the create parameter,
			/// it should create a timer for it. Timer creation is handled by Base class therefore
			/// only passing this parameter to the constructor is enough.
			virtual Base &CreateAnimation(bool create=true) const = 0;
		};

		/// This is the base class for all animations. It handles some common tasks and defines
		/// the animation interface.
		class Base {
		public:
			/// Sets the controller for this animation to the given controller.
			explicit Base(ControllerBase &controller) {
				SetController(controller);
			}

			/// Copies the animation
			Base(const Base &base) {
				SetController(base.GetController());
			}

			/// This constructor creates a new controller depending on the create parameter. Animation has the
			/// right to decline to create a new timer. Animations that does not use timers should ignore create
			/// request without any errors or side effects. If create parameter is true, the controller created
			/// for this object will have dynamic life time. This means, if all animations it has is removed from
			/// it, it will be destroyed.
			explicit Base(bool create=false) {
				if(create) {
					auto timer = new Timer;
					timer->AutoDestruct();
					SetController(*timer);
				}
			}
		
			/// Virtual destructor
			virtual ~Base() {
				if(controller) controller->Remove(*this);
			}

			/// Sets the controller to the given controller.
			virtual void SetController(ControllerBase &controller) {
				if(&controller==this->controller) return;
				if(this->controller) {
					this->controller->Remove(*this);
				}
				this->controller=&controller;
				controller.Add(*this);
			}

			/// Returns whether this animation has a controller
			virtual bool HasController() const { return controller!=nullptr; }

			/// Returns the controller of this animation
			virtual ControllerBase &GetController() const {
#ifndef NDEBUG
				if(!controller) {
					throw std::runtime_error("Animation does not have a controller");
				}
#endif
				return *controller;
			}
			
			/// Removes the controller of this animation.
			virtual void RemoveController() {
				if(controller) {
					controller->Remove(*this);
				}
				controller=nullptr;
			}

			/// This function should progress the animation. Notice that this function is called internally.
			/// Unless a change to the controller has been made and instant update of the animation is required
			/// there is no need to call this function. Returning true from this function denotes that the further
			/// progress is possible. If progress should end, leftover parameter should be set to the amount of
			/// time that cannot be progressed. Progress function should also mind uncontrollable controllers.
			virtual bool Progress(unsigned &leftover) = 0;

			/// Deletes this animation. Please note that some animations are also the animation provider. In these
			/// cases trying to delete the animation will delete the provider as well. This function should be called
			/// instead of delete operator to ensure no such problem occurs.
			virtual void DeleteAnimation() const { 
				delete this; 
			}

		protected:

			/// Controller of this animation
			ControllerBase *controller = nullptr;
		};

		class DiscreteAnimation : public virtual Base {
		public:

			/// Returns the current frame. This function might return NoFrame to denote that the
			/// animation does not contain a frame.
			virtual unsigned CurrentFrame() const = 0;

			/// This variable denotes that this animation has no frame at the moment.
			static const unsigned NoFrame = (unsigned)-1;
		};
		
		/// This is the base class for a single frame in a discreet animation
		class Frame {
		public:
			/// Returns the duration of this frame
			virtual unsigned GetDuration() const = 0;

			/// Returns the starting time of this frame
			virtual unsigned GetStart() const = 0;

			/// Returns the ending time of this frame
			virtual unsigned GetEnd() const {
				return GetStart() + GetDuration();
			}

			/// Returns if the given time is within this frame
			virtual bool IsIn(unsigned time) const {
				return time >= GetStart() && time < GetEnd();
			}
		};

		/**
		 * Provides a discreet animation that is made out of frames. This is an interface and should be 
		 * derived in order to be used.
		 */
		class DiscreteProvider : public virtual Provider {
		public:
			/// Adds a frame to the end of the animation. Frames are designed to be copied, thus they should
			/// be lightweight object.
			virtual void Add(const Frame &frame) = 0;

			/// Adds a frame to the specified point the animation. Frames are designed to be copied, thus they 
			/// should be lightweight object. Before is the index of the frame that this frame will be placed
			/// before. If it is greater or equal to the number of frames, this function will act like add.
			/// Additionally, before could be negative, denoting it will start from the end. -1 is before the
			/// last item.
			virtual void Insert(const Frame &frame, int before) = 0;

			/// Moves the frame with the given index to the specified point the animation. Before is the index 
			/// of the frame that this frame will be placed before. If it is greater or equal to the number of 
			/// frames, this function will move the item to the end. Additionally, before could be negative, 
			/// denoting it will start from the end. -1 is before the last item.
			virtual void MoveBefore(unsigned index, int before) = 0;

			/// Removes the given frame.
			virtual void Remove(unsigned index) = 0;

			/// Returns the frame at specific point
			virtual const Frame &FrameAt(int index) const = 0;
            
			/// Returns the frame at specific point
            const Frame &operator[](int index) const { return FrameAt(index); }

			/// Returns the starting time of the given frame
			virtual unsigned StartOf(unsigned frame) const = 0;

			/// Returns the duration of the animation
			virtual unsigned GetDuration() const = 0;

			/// Returns the duration of the given frame
			virtual unsigned GetDuration(unsigned frame) const = 0;

			/// Clears all frames from this animation
			virtual void Clear() = 0;

			/// Returns number of frames
			virtual int GetCount() const = 0;

		protected:
		};
	}
}
