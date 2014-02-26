#include "LayerMover.h"

using namespace gge::utils;

namespace gge { namespace effects {
	void LayerMover::Setup(Point From, Point To, int Time) {

		from=From;
		current=from;
		to=To;

		Target->BoundingBox.MoveTo(from);
		
		if(Controller)
			Controller->ResetProgress();

		if(Time) {
			speed.x=(float)(to.x-from.x)/Time;
			speed.y=(float)(to.y-from.y)/Time;
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

	animation::ProgressResult::Type LayerMover::Progress() {
		if(current==to)
			return animation::ProgressResult::Finished;

		if(!Controller)
			return animation::ProgressResult::None;

		if(Controller->GetProgress()<0) {
			throw std::runtime_error("LayerMover cannot handle negative animation time.");
		}

		int Time=Controller->GetProgress();

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


		if(from==to)
			return animation::ProgressResult::Finished;

		return animation::ProgressResult::None;
	}

	LayerMover::LayerMover( LayerBase *Target, animation::Timer &controller, bool owner ) :
		Base(controller, owner),
		speed(0,0),
		current(Target->BoundingBox.TopLeft()),
		Target(Target)
	{
	}

	LayerMover::LayerMover( LayerBase &Target, animation::Timer &controller, bool owner ) :
		Base(controller, owner),
		speed(0,0),
		current(Target.BoundingBox.TopLeft()),
		Target(&Target) 
	{
	}

	LayerMover::LayerMover( LayerBase *Target, bool create/*=false*/ ) : 
	Target(Target), Base(create)
	{

	}

	LayerMover::LayerMover( LayerBase &Target, bool create/*=false*/ ) : 
	Target(&Target), Base(create)
	{

	}

} }
