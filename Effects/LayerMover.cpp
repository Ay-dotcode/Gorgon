#include "LayerMover.h"

using namespace gge::utils;

namespace gge { namespace effects {
	void LayerMover::Setup(Point From, Point To, int Time) {

		from=From;
		current=from;
		to=To;

		Target->BoundingBox.MoveTo(from);
		this->progressed=0;

		if(Time) {
			speed.x=(float)(to.x-from.x)/Time;
			speed.y=(float)(to.y-from.y)/Time;
			this->Play();
		} else {
			speed.x=0;
			speed.y=0;
			current=from=to;
			Target->BoundingBox.MoveTo(to);
		}
	}

	void LayerMover::Setup( utils::Point To, int Time ) {
		Setup(Point((int)current.x,(int)current.y), To, Time);
	}

	bool LayerMover::isFinished() {
		return current==to;
	}

	void LayerMover::Process(int Time) {
		if(from.x>to.x) {
			current.x=from.x+Time*speed.x;

			if(current.x<to.x)
				current.x=to.x;
		} else {
			current.x=from.x+Time*speed.x;
			if(current.x>to.x)
				current.x=to.x;
		}


		if(from.y>to.y) {
			current.y=from.y+Time*speed.y;

			if(current.y<to.y)
				current.y=to.y;
		} else {
			current.y=from.y+Time*speed.y;
			if(current.y>to.y)
				current.y=to.y;
		}
		Target->BoundingBox.MoveTo(current);

	}

	LayerMover::LayerMover( LayerBase *Target ) :
	speed(0,0),
		current(Target->BoundingBox.TopLeft()),
		Target(Target), FinishedEvent("Finished", this) {
		AnimatorBase::FinishedEvent.DoubleLink(FinishedEvent);
	}

	LayerMover::LayerMover( LayerBase &Target ) :
	speed(0,0),
		current(Target.BoundingBox.TopLeft()),
		Target(&Target), FinishedEvent("Finished", this) {
		AnimatorBase::FinishedEvent.DoubleLink(FinishedEvent);
	}

} }
