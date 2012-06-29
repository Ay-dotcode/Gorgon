
#include "Animation.h"
#include "../Utils/Collection.h"
#include "../Utils/BasicMath.h"

using namespace gge::utils;
using namespace gge::graphics;

namespace gge { namespace animation {

	Collection<Timer> Timers;
	Collection<Base> Animations;

	unsigned LastTick;

	void Animator_Signal() {
		unsigned progressed=Main.CurrentTime-LastTick;
		if(progressed==0) return;

		for(Collection<Timer>::Iterator i=Timers.First();i.IsValid();i.Next())
			i->Progress(progressed);

		for(Collection<Base>::Iterator i=Animations.First();i.IsValid();i.Next()) {
			if(i->HasController()) {
				ProgressResult::Type r=i->Progress();

				if(r!=ProgressResult::None)
					i->GetController().Obtained(r, i);
			}
		}

		LastTick=Main.CurrentTime;
	}

	void Initialize(GGEMain &main) {
		LastTick=main.CurrentTime;
	}


	Timer::Timer() : progress(0) {
		Timers.Add(this);
	}

	Timer::~Timer() {
		Timers.Remove(this);
	}

	void Timer::Progress( unsigned timepassed ) {
		progress += timepassed;
	}


	Controller::Controller() : Timer(), 
		ispaused(false), isfinished(false), speed(1.f),
		Finished("Finished", this),
		Paused("Paused", this), pauseat(-1)
	{

	}

	void Controller::Progress( unsigned timepassed ) {
		if(!ispaused && !isfinished) {
			if(Round(mprogress)!=progress)
				mprogress=progress;

			mprogress+=(float)timepassed*speed;
			progress=(int)Round<double>(mprogress);

			if(pauseat>0 && speed>0 && pauseat<=progress) {
				ispaused=true;
				pauseat=-1;
				Paused(source_param(NULL));
			}
			else if(pauseat>=0 && speed<0 && pauseat>=progress) {
				ispaused=true;
				pauseat=-1;
				Paused(source_param(NULL));
			}
		}
	}

	void Controller::Play() {
		ispaused=false; 
		isfinished=false;
		mprogress=0;
		progress=0;
	}

	void Controller::Obtained( ProgressResult::Type r, Base &source ) {
		if(r==ProgressResult::Finished) {
			if(!isfinished) {
				isfinished=true;
				Finished(source_param(&source));
			}
		}
		if(r==ProgressResult::Pause) {
			ispaused=true;
			Paused(source_param(&source));
		}

		if(Round(mprogress)!=progress)
			mprogress=progress;
	}

	void Controller::Pause() {
		ispaused=true;
	}

	void Controller::ResetProgress() {
		isfinished=false;
		if(speed<0) {
			mprogress=-1;
			progress=-1;
		}
		else {
			mprogress=0;
			progress=0;
		}
	}


	Base::Base(Timer &Controller, bool owner) : Controller(&Controller), owner(owner) {
		Animations.Add(this);
	}

	Base::Base(bool create) : Controller(NULL) {
		Animations.Add(this);

		if(create) {
			Controller=new Timer();
			owner=true;
		}
		else {
			owner=false;
		}
	}

	void Base::SetController( Timer &controller, bool owner ) {
		if(this->owner && Controller)
			delete Controller;

		Controller=&controller; 
		this->owner=owner;
		
		ProgressResult::Type r=Progress();

		if(r!=ProgressResult::None)
			controller.Obtained(r, *this);
	}

	Base::~Base() {
		Animations.Remove(this);
		if(owner && Controller)
			delete Controller;
	}



	void DiscreteController::Progress(unsigned timepassed) {
		Controller::Progress(timepassed);

		int t=GetProgress();
		int tl=info.GetDuration();

		if(tl==0)
			currentframe=-1;
		else {
			if(!islooping && ( (t>=tl && speed>=0) || (t<=0 && speed<0) )  ) {
				isfinished=true;
				Finished(source_param(NULL));

				if(speed>0)
					currentframe=info.GetNumberofFrames()-1;
				else
					currentframe=0;
			}
			else {
				currentframe=FrameAt(PositiveMod(t,tl));
			}
		}

		if(pauseatframe>0 && speed>0 && currentframe>=pauseatframe) {
			ispaused=true;
			pauseatframe=-1;
			Paused(source_param(NULL));
		}
		else if(pauseatframe>0 && speed<0 && currentframe<=pauseatframe) {
			ispaused=true;
			pauseatframe=-1;
			Paused(source_param(NULL));
		}
	}

	void DiscreteController::Goto(int Frame) {
		if( utils::InRange(Frame,0,GetNumberofFrames()-1) ) {
			if(speed>0)
				SetProgress(info.StartOf(Frame));
			else
				SetProgress(info.EndOf(Frame));

			currentframe=Frame;
		}
		else {
			SetProgress(0);
			currentframe=-1;
		}
	}

	void DiscreteController::Play() {
		ispaused=false;
		ResetProgress();
	}

	void DiscreteController::ResetProgress() {
		isfinished=false;
		if(speed>=0) {
			SetProgress(0);
		}
		else {
			SetProgress(info.GetDuration()-1);
		}
	}


} }