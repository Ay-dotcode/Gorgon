#include "Controllers.h"

namespace Gorgon { namespace Audio {
	
	Controller::Controller() {
		std::lock_guard<std::mutex> guard(internal::ControllerMtx);
		
		internal::Controllers.Add(this);
	}
	
	Controller::Controller(Containers::Wave &wavedata) : wavedata(&wavedata) { 
		std::lock_guard<std::mutex> guard(internal::ControllerMtx);
		
		internal::Controllers.Add(this);
		datachanged(); 
	}

	Controller::~Controller() {
		std::lock_guard<std::mutex> guard(internal::ControllerMtx);
		
		internal::Controllers.Remove(this);
	}

	void Controller::ReleaseData() {
		std::lock_guard<std::mutex> guard(internal::ControllerMtx);
		
		wavedata = nullptr;
		datachanged();
	}
	
	void Controller::SetData(Containers::Wave &wavedata) {
		std::lock_guard<std::mutex> guard(internal::ControllerMtx);
		
		this->wavedata = &wavedata;
		datachanged();
	}
	
	BasicController &BasicController::Play() {		
		if(wavedata) {
			if( position >= wavedata->GetLength() ) {
				position = 0;
			}
		}
		else {
			position = 0;
		}
		
		playing = true;
		looping = false;
		
		return *this;
	}

	BasicController &BasicController::Loop() {
		playing = true;
		looping = true;
		
		return *this;
	}
	
	BasicController &BasicController::Pause() {
		playing = false;
		
		return *this;
	}

	BasicController &BasicController::Reset() {
		position = 0;
		
		return *this;
	}
	
	BasicController &BasicController::Seek(float target) {
		position = target;
		
		return *this;
	}

	BasicController &BasicController::SeekTo(float target) {
		if(wavedata) {
			position = wavedata->GetLength() * target;
		}
		else {
			position = 0;
		}
		
		return *this;
	}
	
	BasicController &BasicController::SetVolume(float volume) {
		this->volume = volume;
		
		return *this;
	}
		
	float BasicController::GetVolume() const {
		return volume;
	}

	float BasicController::GetDuration() const {
		if(wavedata)
			return wavedata->GetLength();
		else
			return 0;
	}

	float BasicController::GetCurrentTime() const {
		return position;
	}

	float BasicController::GetCurrentFraction() const {
		if(wavedata)
			return position / wavedata->GetLength();
		else
			return 0;
	}

	bool BasicController::IsFinished() const {
		return (position >= wavedata->GetLength() && !playing);
	}

	bool BasicController::IsLooping() const {
		return looping;
	}

	bool BasicController::IsPlaying() const {
		return playing;
	}

	namespace internal {
		Containers::Collection<Controller> Controllers;
		std::mutex ControllerMtx;
	}
} }