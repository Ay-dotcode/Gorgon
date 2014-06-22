
#include "../Animation.h"
#include "../Time.h"


namespace Gorgon { namespace Animation {

	Containers::Collection<Timer> Timers;
	Containers::Collection<Base>  Animations;

	unsigned LastTick;

	void Animate() {
		if(Time::DeltaTime()==0) return;

		auto progress=Time::DeltaTime();

		for(auto &timer : Timers)
			timer.Progress(progress);

		for(auto &base : Animations) {
			if(base.HasController()) {
				auto leftover=base.Progress();

				if(leftover) {
					base.GetController().Finished(leftover);
				}
			}
		}
	}

	void Initialize() {
		/// Nothing to do right now
	}


	Timer::Timer() {
		Timers.Add(this);
	}

	Timer::~Timer() {
		Timers.Remove(this);
	}

	void Timer::Progress( unsigned timepassed ) {
		progress += timepassed;
	}


	Controller::Controller() : Timer(),
		FinishedEvent(*this)
	{ }

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
		floatprogress=0;
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