#pragma once

#include "../Utils/GGE.h"
#include "../Resource/GRE.h"
#include "../Engine/Animator.h"
#include "../Engine/Graphics.h"
#include "../Engine/Layer.h"

using namespace gge;
using namespace gre;

namespace geffects {

	////This effect resizes a given layer. Resize operation is animated from a given
	/// value to another one
	class LayerResizer : public AnimatorBase {
	public:
		////This event is fired when the animation
		/// completes
		EventChain<LayerResizer, empty_event_params> FinishedEvent;

		////Target of this effect
		LayerBase *Target;

		////Initializes the effect
		LayerResizer(LayerBase *Target) :
			speed(0,0 , 0,0),
			current((float)Target->X, (float)Target->Y, (float)Target->W, (float)Target->H),
			Target(Target),
			FinishedEvent("Finished", this)
		{
			AnimatorBase::FinishedEvent=FinishedEvent;
		}

		////Initializes the effect
		LayerResizer(LayerBase &Target) :
			speed(0,0 , 0,0),
			current((float)Target.X, (float)Target.Y, (float)Target.W, (float)Target.H),
			Target(&Target),
			FinishedEvent("Finished", this)
		{
			AnimatorBase::FinishedEvent=FinishedEvent;
		}
		////Sets source and destination to the given values and allows time duration to reach the
		/// destination
		void Setup(gge::Rectangle From, gge::Rectangle To, int Time);
		////Sets current destination to the given value and allows time duration to reach it
		void Setup(gge::Rectangle To, int Time) { Setup(gge::Rectangle(Round(current.Left), Round(current.Top), Round(current.Width), Round(current.Height)), To, Time); }

	protected:
		Rectangle2D from;
		Rectangle2D to;
		Rectangle2D current;
		Rectangle2D speed;

		virtual bool isFinished();
		virtual void Process(int Time);
	};
}
