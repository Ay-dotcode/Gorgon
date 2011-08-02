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

	class AnimationBase;

	class AnimationTimer {
	public:
		enum Type {
			Continous,
			Discrete
		};

		AnimationTimer();

		virtual ~AnimationTimer();

		virtual void Progress(unsigned timepassed);
		virtual void Obtained(ProgressResult::Type r, AnimationBase &source) 
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

		source_param(AnimationBase *source) : source(source)
		{ }

		AnimationBase *source;
	};

	class AnimationController : public AnimationTimer {
	public:

		AnimationController();

		virtual void Progress(unsigned timepassed);
		virtual void Obtained(ProgressResult::Type r, AnimationBase &source);

		virtual void Pause();
		virtual void Resume() { ispaused=false; }

		virtual void Play();

		virtual void SetSpeed(float speed) { this->speed=speed; }
		virtual float GetSpeed() { return speed; }

		virtual void Reverse() { speed=-speed; }

		bool IsPaused() { return ispaused; }
		bool IsFinished() { return isfinished; }

		virtual void ResetProgress();

		void SetPauseAt(int t) {
			pauseat=t;
		}
		void RemovePauseAt() {
			pauseat=-1;
		}
		virtual void SetProgress(int progress) { mprogress=this->progress=progress; }

		utils::EventChain<AnimationController, source_param> Finished;
		utils::EventChain<AnimationController, source_param> Paused;

	protected:
		bool ispaused;
		float speed;
		int pauseat;
		bool isfinished;
		double mprogress;
	};

	class AnimationProvider {
	public:
		virtual AnimationBase &CreateAnimation(AnimationTimer &controller, bool owner=false) = 0;
		virtual AnimationBase &CreateAnimation(bool create=false) = 0;
	};

	class DiscreteAnimationProvider : virtual public AnimationProvider {
	public:
		//if there is a single frame, duration should be 0
		virtual int GetDuration() const					= 0;
		virtual int GetDuration(unsigned Frame) const	= 0;
		virtual int GetNumberofFrames() const			= 0;

		//Caller is responsible to supply a time between 0 and GetDuration()-1, if no frame exists it should return -1
		virtual int		 FrameAt(unsigned Time) const	= 0; 
		//Should always return a time between 0 and GetDuration unless Frame does not exists it should return -1
		virtual int		 StartOf(unsigned Frame) const	= 0; 
		virtual	int		 EndOf(unsigned Frame) const { return StartOf(Frame)+GetDuration(Frame); }
	};

	class DiscreteController : public AnimationController {
	public:
		DiscreteController(DiscreteAnimationProvider &info) : AnimationController(), 
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

		virtual Type GetType() const { return AnimationTimer::Discrete; }
		virtual void Progress(unsigned timepassed);

		virtual void Play();
		virtual void ResetProgress();

	protected:
		bool islooping;
		int pauseatframe;
		int currentframe;
		DiscreteAnimationProvider &info;
	};

	class AnimationBase {
	public:
		AnimationBase(AnimationTimer &Controller, bool owner=false);
		explicit AnimationBase(bool create=false);
		
		virtual ~AnimationBase();

		virtual void SetController(AnimationTimer &controller, bool owner=false);
		bool HasController() { return Controller!=NULL; }
		AnimationTimer &GetController() { return *Controller; }
		void RemoveController() { Controller=NULL; }


		virtual ProgressResult::Type Progress() = 0;
		virtual void DeleteAnimation() { delete this; }


	protected:
		AnimationTimer *Controller;
		bool owner;
	};
	
	class RectangularGraphic2DAnimation : virtual public graphics::RectangularGraphic2D, virtual public AnimationBase {

	};

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

 	class RectangularGraphic2DSequenceProvider : virtual public DiscreteAnimationProvider {
	public:
		virtual RectangularGraphic2DAnimation &CreateAnimation(AnimationTimer &controller, bool owner=false) = 0;
		virtual RectangularGraphic2DAnimation &CreateAnimation(bool create=false) = 0;
		virtual graphics::RectangularGraphic2D &ImageAt(int time)=0;
	};
} }
