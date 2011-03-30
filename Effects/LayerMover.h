#pragma once

#include "../Utils/GGE.h"
#include "../Resource/GRE.h"
#include "../Engine/Animator.h"
#include "../Engine/Graphics.h"
#include "../Engine/Layer.h"

using namespace gge;
using namespace gre;

namespace geffects {

	////This effect moves a given layer. Moving operation is animated from a given
	/// value to another one
	class LayerMover : public AnimatorBase {
	public:
		////This event is fired when the animation
		/// completes
		EventChain<LayerMover, empty_event_params> FinishedEvent;

		////Target of this effect
		LayerBase *Target;

		////Initializes the effect
		LayerMover(LayerBase *Target) :
			speed(0,0),
			current((float)Target->X, (float)Target->Y),
			Target(Target), FinishedEvent("Finished", this)
		{
			AnimatorBase::FinishedEvent.DoubleLink(FinishedEvent);
		}
		////Initializes the effect
		LayerMover(LayerBase &Target) :
			speed(0,0),
			current((float)Target.X, (float)Target.Y),
			Target(&Target), FinishedEvent("Finished", this)
		{
			AnimatorBase::FinishedEvent.DoubleLink(FinishedEvent);
		}
		////Sets source and destination to the given values and allows time duration to reach the
		/// destination
		void Setup(Point From, Point To, int Time);
		////Sets current destination to the given value and allows time duration to reach it
		void Setup(Point To, int Time) { Setup(Point((int)current.x,(int)current.y), To, Time); }

	protected:
		Point2D from;
		Point2D to;
		Point2D current;
		Point2D speed;

		virtual bool isFinished();
		virtual void Process(int Time);
	};
}
