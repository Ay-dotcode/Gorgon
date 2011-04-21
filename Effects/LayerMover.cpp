#include "LayerMover.h"

namespace gge { namespace effects {
	void LayerMover::Setup(Point From, Point To, int Time) {

		from=From;
		current=from;
		to=To;

		Target->X=(int)from.x;
		Target->Y=(int)from.y;
		this->progressed=0;

		if(Time) {
			speed.x=(float)(to.x-from.x)/Time;
			speed.y=(float)(to.y-from.y)/Time;
			this->Play();
		} else {
			speed.x=0;
			speed.y=0;
			current=from=to;
			Target->X=(int)to.x;
			Target->Y=(int)to.y;
		}
	}

	bool LayerMover::isFinished() {
		return current.x==to.x && current.y==to.y;
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
		Target->X=(int)current.x;


		if(from.y>to.y) {
			current.y=from.y+Time*speed.y;

			if(current.y<to.y)
				current.y=to.y;
		} else {
			current.y=from.y+Time*speed.y;
			if(current.y>to.y)
				current.y=to.y;
		}
		Target->Y=(int)current.y;

	}
} }
