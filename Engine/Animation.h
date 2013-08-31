//This file contains core of animation system
// all animations exposes interfaces defined here
// and 

#pragma once

#include "Graphic2D.h"
#include "GGEMain.h"
#include <stdexcept>

namespace gge { namespace animation {

	void Initialize(GGEMain &main);

	class ProgressResult {
	public:
		enum Type {
			None,
			Pause,
			Finished,
		};
	};

	class Base;

	class Timer {
	public:
		enum Type {
			Continous,
			Discrete
		};

		Timer();

		virtual ~Timer();

		virtual void Progress(unsigned timepassed);
		virtual void Obtained(ProgressResult::Type r, Base &source) 
		{}

		virtual void ResetProgress() { progress=0; }
		virtual void SetProgress(int progress) { this->progress=progress; }
		virtual int GetProgress() const { return progress; }

		virtual Type GetType() const { return Continous; }

	protected:
		int progress;
	};

	class source_param {
	public:

		source_param(Base *source) : source(source)
		{ }

		Base *source;
	};

	class Controller : public Timer {
	public:

		Controller();
		virtual ~Controller() {}

		virtual void Progress(unsigned timepassed);
		virtual void Obtained(ProgressResult::Type r, Base &source);

		virtual void Pause();
		virtual void Resume() { ispaused=false; }

		virtual void Play();

		virtual void SetSpeed(float speed) { this->speed=speed; }
		virtual float GetSpeed() { return speed; }

		virtual void Reverse() { speed=-speed; }

		bool IsPaused() { return ispaused; }
		bool IsFinished() { return isfinished; }
		void ClearFinished() { isfinished=false; }

		virtual void ResetProgress();

		void SetPauseAt(int t) {
			pauseat=t;
		}
		int GetPauseAt() const {
			return pauseat;
		}
		void RemovePauseAt() {
			pauseat=-1;
		}
		virtual void SetProgress(int progress) { mprogress=this->progress=progress; }

		utils::EventChain<Controller, source_param> Finished;
		utils::EventChain<Controller, source_param> Paused;

	protected:
		bool ispaused;
		float speed;
		int pauseat;
		bool isfinished;
		double mprogress;
	};

	class Provider {
	public:
		virtual ~Provider() { }
		
		virtual Base &CreateAnimation(Timer &controller, bool owner=false) = 0;
		virtual Base &CreateAnimation(bool create=false) = 0;
	};

	class DiscreteProvider : virtual public Provider {
	public:
		//if there is a single frame, duration should be 0
		virtual int GetDuration() const					= 0;
		virtual int GetDuration(unsigned Frame) const	= 0;
		virtual int GetNumberofFrames() const			= 0;
    virtual float GetFPS() const {
      return GetNumberofFrames() / (GetDuration() / 1000.f);
    }
    
    

		//Caller is responsible to supply a time between 0 and GetDuration()-1, if no frame exists it should return -1
		virtual int		 FrameAt(unsigned Time) const	= 0; 
		//Should always return a time between 0 and GetDuration unless Frame does not exists it should return -1
		virtual int		 StartOf(unsigned Frame) const	= 0; 
		virtual	int		 EndOf(unsigned Frame) const { return StartOf(Frame)+GetDuration(Frame); }
	};

	class DiscreteController : public Controller {
	public:
		DiscreteController(DiscreteProvider &info) : Controller(), 
			islooping(true), info(info), pauseatframe(-1), currentframe(-1)
		{ }


		bool IsLooping() const { return islooping; }
		void SetLoop(bool loop) { islooping=loop; }

		void Goto(int Frame);
		//-1 is invalid frame
		int CurrentFrame() const { return currentframe; }

		void SetPauseAtFrame(int Frame) { pauseatframe=Frame; }
		int  GetPauseAtFrame() const { return pauseatframe; }
		void CancelPauseAtFrame() { pauseatframe=-1; }

		virtual int FrameAt(unsigned Time) const { 
			return info.FrameAt(Time); 
		}

		virtual int StartOf(int Frame) const { 
			if(!utils::InRange(Frame, 0, GetDuration())) 
				return 0; 

			return info.StartOf(Frame); 
		}

		virtual int EndOf(int Frame) const { 
			if(!utils::InRange(Frame, 0, GetDuration())) 
				return GetNumberofFrames(); 

			return info.EndOf(Frame); 
		}

		virtual int GetDuration() const { return info.GetDuration(); }
		virtual int GetNumberofFrames() const { return info.GetNumberofFrames(); }

		virtual Type GetType() const { return Timer::Discrete; }
		virtual void Progress(unsigned timepassed);

		virtual void Play();
		virtual void ResetProgress();

		//finished and paused events are created using info
		virtual void Obtained(ProgressResult::Type r, Base &source) { }

	protected:
		bool islooping;
		int pauseatframe;
		int currentframe;
		DiscreteProvider &info;
	};

	class Base {
	public:
		Base(Timer &Controller, bool owner=false);
		explicit Base(bool create=false);
		
		virtual ~Base();

		virtual void SetController(Timer &controller, bool owner=false);
		bool HasController() { return Controller!=NULL; }
		Timer &GetController() { return *Controller; }
		void RemoveController() { Controller=NULL; }


		virtual ProgressResult::Type Progress() = 0;
		virtual void DeleteAnimation() { 
			delete this; 
		}


	protected:
		Timer *Controller;
		bool owner;
	};
#pragma warning(push)
#pragma warning(disable:4250)
	class Basic2DAnimation : virtual public graphics::Graphic2D, virtual public Base {

	};

	class RectangularGraphic2DAnimation : virtual public Basic2DAnimation, virtual public graphics::RectangularGraphic2D {

	};
#pragma warning(pop)

/*
	class Graphic2DAnimation : public virtual AnimationBase, public graphics::RectangularGraphic2D {
	protected:

		Graphic2DAnimation() : AnimationBase() { }
		Graphic2DAnimation(AnimationTimer &Controller) : AnimationBase(Controller) { }
	};*/

	//class Graphic2DAnimationProvider : public AnimationProvider {
	//public:
	//	virtual Graphic2DAnimation &CreateAnimation(AnimationTimer &controller, bool owner=false) = 0;
	//	virtual Graphic2DAnimation &CreateAnimation(bool create=false) = 0;
	//};

	class Basic2DAnimationProvider : virtual public Provider {
	public:
		virtual Basic2DAnimation &CreateAnimation(Timer &controller, bool owner=false) = 0;
		virtual Basic2DAnimation &CreateAnimation(bool create=false) = 0;
	};

	class RectangularGraphic2DAnimationProvider : virtual public Basic2DAnimationProvider {
	public:
		virtual RectangularGraphic2DAnimation &CreateAnimation(Timer &controller, bool owner=false) = 0;
		virtual RectangularGraphic2DAnimation &CreateAnimation(bool create=false) = 0;
	};

 	class RectangularGraphic2DSequenceProvider : virtual public RectangularGraphic2DAnimationProvider, virtual public DiscreteProvider {
	public:
		virtual RectangularGraphic2DAnimation &CreateAnimation(Timer &controller, bool owner=false) = 0;
		virtual RectangularGraphic2DAnimation &CreateAnimation(bool create=false) = 0;
		virtual graphics::RectangularGraphic2D &ImageAt(int time)=0;
	};

	extern utils::Collection<Base> Animations;
} }
