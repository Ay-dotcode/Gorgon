#include <stdexcept>

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
				unsigned leftover=0;

				if(!base.Progress(leftover)) {
					if(base.GetController().Finished(leftover)) {
						if(!base.Progress(leftover)) {
							base.GetController().Finished(leftover);
						}
					}
				}
			}
		}
	}

	Timer::Timer() {
		Timers.Add(this);
	}

	Timer::~Timer() {
		Timers.Remove(this);
	}

	bool Timer::Progress( unsigned timepassed ) {
		progress += timepassed;
		return false;
	}

	Controller::Controller() : Timer(),
		FinishedEvent(*this)
	{ }

	void Controller::Progress( unsigned timepassed ) {
		if(!ispaused && !isfinished) {
			floatprogress+=(float)timepassed*speed;

			if(floatprogress<0) {
				if(islooping) {
					if(length) {
						floatprogress = length-floatprogress;
					}
					else {
						floatprogress=0;
						isfinished=true;
#ifndef NDEBUG
						throw std::runtime_error("! Gorgon::Animation: Loop is required with negative speed but length is not set.");
#endif
					}
				}
				else {
					floatprogress=0;
					isfinished=true;
				}
				FinishedEvent();
			}

			progress=(unsigned)std::round(floatprogress);
		}
	}

	void Controller::Play() {
		if(isfinished) {
			if(speed>=0) {
				floatprogress=0;
				progress=0;
				ispaused=false;
				isfinished=false;
			}
			else if(length) {
				floatprogress=length;
				progress=length;
				ispaused=false;
				isfinished=false;
			}
			else {
#ifndef NDEBUG
				throw std::runtime_error("! Gorgon::Animation: \"Play\" is requested with negative speed but length is not set.");
#endif
			}
		}
		else {
			ispaused=false;
		}
		islooping=false;
	}

	void Controller::Reset() {
		speed=1.0;

		floatprogress=0;
		progress=0;
		ispaused=false;
		isfinished=false;
	}

	bool Controller::Finished( unsigned leftover ) {
		if(islooping) {
			progress=leftover;
			floatprogress=progress;
		}
		else {
			progress-=leftover;
		}

		FinishedEvent();
	}

	void Controller::Pause() {
		ispaused=true;
	}


	Base::Base(Timer &controller, bool owner) : controller(&controller), owner(owner) {
		Animations.Add(this);
	}

	Base::Base(bool create) {
		Animations.Add(this);

		if(create) {
			controller=new Timer();
			owner=true;
		}
		else {
			owner=false;
		}
	}

	void Base::RemoveMe() {
		Animations.Remove(this);
	}

	void Base::SetController( Timer &controller, bool owner ) {
		RemoveController();

		this->controller=&controller; 
		this->owner=owner;

		unsigned leftover;
		if(!this->Progress(leftover)) {
			controller.Finished(leftover);
		}
	}

	Base::~Base() {
		RemoveController();
	}

} }