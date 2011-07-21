#include "Animator.h"
#include "GGEMain.h"

using namespace gge::utils;

namespace gge {
	Collection<AnimatorBase> Animators;

	void Animator_Signal(IntervalObject *interval, void *data) {
		for(Collection<AnimatorBase>::Iterator i=Animators.First();i.isValid();i.Next())
			i->Progress();
	}

	void InitializeAnimation(GGEMain *main) {
		Main.RegisterInterval(0, NULL, Animator_Signal);
	}

	AnimatorBase::AnimatorBase() : 
		FinishedEvent("Finished", this),
		InterruptedEvent("Interrupted", this)
	{
		Animators.Add(this);

		///*Assigning default values
		pauseat=-1;
		pausedat=Main.CurrentTime;
		speed=1;
		lasttick=Main.CurrentTime;
		ispaused=false;
		progressed=0;
	}

	AnimatorBase::~AnimatorBase() {
		Animators.Remove(this);
	}

	int AnimatorBase::currentTime() {
		return (int)progressed;
	}

	bool AnimatorBase::isPlaying() {
		return !ispaused && !isFinished();
	}

	void AnimatorBase::Pause() {
		ispaused=true;
		pausedat=Main.CurrentTime;
	}

	void AnimatorBase::PauseAt(int ms) {
		pauseat=ms;
	}

	void AnimatorBase::setSpeed(float Speed) {
		speed=Speed;
	}

	void AnimatorBase::Progress() {
		if(isFinished()) return;

		if(!ispaused) {
			ProgressBy(Main.CurrentTime-lasttick);
			lasttick=Main.CurrentTime;
		}
	}

	void AnimatorBase::ProgressBy(int ms) {
		progressed+=(float)ms*speed;
		Process((int)progressed);

		if(!ispaused) {
			if(progressed>pauseat && pauseat>0) {
				pauseat=-1;
				Pause();
				InterruptedEvent();
			}
		}

		if(isFinished()) {
			FinishedEvent();
		}
	}

	void AnimatorBase::ProgressTo(float ms) {
		progressed=ms;
		Process((int)progressed);

		if(!ispaused) {
			if(progressed>pauseat && pauseat>0) {
				pauseat=-1;
				Pause();
				InterruptedEvent();
			}
		}

		if(isFinished()) {
			FinishedEvent();
		}
	}

	void AnimatorBase::Play() {
		lasttick=Main.CurrentTime;
		ispaused=false;
	}

	DiscreteAnimatorBase::DiscreteAnimatorBase() : 
		AnimatorBase(),
		FrameChangedEvent("framechanged",this),
		FrameInterruptedEvent("frameinterrupted",this)
	{
		totalframes=0;
		frametimings=NULL;
		framedurations=NULL;
		currentframe=0;
		averagedurations=0;
		duration=0;
		frametimings=NULL;
		EventOnFrameChange=false;
		progressed=0;
		gobackwards=false;
		islooping=true;
	}

	void DiscreteAnimatorBase::GotoFrame(int Frame) {
#ifdef _DEBUG
		if(Frame<0) {
			os::DisplayMessage("Discreate animation","Negative frame requested!");
			assert(0);
			return;
		}
		if(Frame>totalframes) {
			os::DisplayMessage("Discreate animation","Specified frame does not exists!");
			assert(0);
			return;
		}
#endif

		AnimatorBase::ProgressTo((float)frametimings[Frame]);
	}

	void DiscreteAnimatorBase::setLoop(bool loop) {
		islooping=loop;
	}

	void DiscreteAnimatorBase::Reverse() {
		gobackwards=!gobackwards;
		speed=-speed;

		//reverses the duration passed in this frame
		ProgressTo(frametimings[currentframe]+framedurations[currentframe]-(progressed-frametimings[currentframe]));
	}
	void DiscreteAnimatorBase::GoForwards() {
		if(gobackwards && progressed==duration-1) {
			progressed=0;
			currentframe=0;
		}

		if(speed<0)
			speed=-speed;
		gobackwards=false;
	}
	void DiscreteAnimatorBase::GoBackwards() {
		if(!gobackwards && progressed==0) {
			progressed=(float)(duration-1);
			currentframe=totalframes-1;
		}

		if(speed>0)
			speed=-speed;
		gobackwards=true;
	}

	bool DiscreteAnimatorBase::isLooping() {
		return islooping;
	}
	bool DiscreteAnimatorBase::isBackwards() {
		return gobackwards;
	}
	int  DiscreteAnimatorBase::currentFrame() {
		return currentframe;
	}
	void DiscreteAnimatorBase::setTotalFrames(int total) {
#ifdef _DEBUG
		if(total==0) {
			os::DisplayMessage("Discrete animation","Empty total frames requested!");
			assert(0);
		}
#endif

		///*Startup
		totalframes=total;
		framedurations=FrameDurations();

		if(frametimings)
			delete[] frametimings;
		frametimings=new int[total+1];

		///*Calculate frame timings
		int i;
		duration=0;
		for(i=0;i<total;i++) {
			frametimings[i]=duration;
			duration+=framedurations[i];
		}
		frametimings[i]=duration;

#ifdef _DEBUG
		if(duration==0) {
			os::DisplayMessage("Discrete animation","Empty duration animation!");
			assert(0);
		}
#endif


		///*Calculate statistics
		averagedurations=duration/total;

		Progress();
	}
	void DiscreteAnimatorBase::Process(int time) {
		int i;
		if(duration==0)
			return;

#ifdef _DEBUG
		if(averagedurations==0) {
			os::DisplayMessage("Discrete animation","Trying to process an empty animation!");
			assert(0);
		}
#endif
		///*Checking time boundaries
		//when going back
		if(time<0 && gobackwards) {
			if(islooping) {
				if(time<-duration)
					time=time % duration;

				progressed=(float)((duration-1)+time);
			} else {
				progressed=0;
				Pause();
			}
		}//when going forward
		else if(time>=duration && !gobackwards) {
			if(islooping) {
				time=time % duration;
				progressed=(float)time;
			} else {
				progressed=(float)(duration-1);
				Pause();
			}
		}
		time=(int)progressed;

		///*finding current frame
		//statistics is used to speedup search
		int guessedframe=time/averagedurations;
		int targetframe;
#ifdef _DEBUG
		if(guessedframe<0) {
			os::DisplayMessage("Discrete animation","Guessed frame is lower than 0??");
			guessedframe=0;
		} else if(guessedframe>=totalframes) {
			guessedframe=totalframes-1;
		}
#endif

		targetframe=0;
		//guessedframe=0;
		//if guess is higher
		if(frametimings[guessedframe]>time) {
			for(i=guessedframe-1;i>=0;i--)
				if(frametimings[i]<time) {
					targetframe=i;
					break;
				}
		}//if guess is lower
		else if(frametimings[guessedframe+1]<time) {
			for(i=guessedframe+1;i<totalframes;i++)
				if(frametimings[i+1]>time) {
					targetframe=i;
					break;
				}
		}//if exact find
		else {
			targetframe=guessedframe;
		}

		//frame is changed
		if(targetframe!=currentframe) {
			frame_changed_event_params p(currentframe,targetframe);
			FrameChangedEvent(p);
		}
#ifdef _DEBUG
		if(targetframe<0) {
			os::DisplayMessage("Discrete animation","Target frame is lower than 0!");
			guessedframe=0;
		} else if(targetframe>=totalframes) {
			os::DisplayMessage("Discrete animation","Target frame is higher than totalframes!");
			assert(0);
		}
#endif

		currentframe=targetframe;

		//checking for pause at frame
		if(gobackwards && pauseatframe>targetframe) {
			FrameInterruptedEvent();
			pauseatframe=-1;
		}
		if(!gobackwards && pauseatframe<targetframe && pauseatframe>-1) {
			FrameInterruptedEvent();
			pauseatframe=-1;
		}

		ProcessFrame(currentframe);
	}
	void DiscreteAnimatorBase::PauseAtFrame(int frame) {
		pauseatframe=frame;
	}
	void DiscreteAnimatorBase::ProcessFrame(int frame) { }
}