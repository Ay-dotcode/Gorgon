#pragma once

#include "../Resource/GRE.h"
#include "../Engine/Animator.h"
#include "../Engine/Graphics.h"
#include "../Engine/Layer.h"

namespace gge { namespace effects {

	////This effect moves a given layer. Moving operation is animated from a given
	/// value to another one
	class LayerMover : public gge::AnimatorBase {
	public:
		////This event is fired when the animation
		/// completes
		utils::EventChain<LayerMover> FinishedEvent;

		////Target of this effect
		LayerBase *Target;

		////Initializes the effect
		LayerMover(LayerBase *Target);
		////Initializes the effect
		LayerMover(LayerBase &Target);
		////Sets source and destination to the given values and allows time duration to reach the
		/// destination
		void Setup(utils::Point From, utils::Point To, int Time);
		////Sets current destination to the given value and allows time duration to reach it
		void Setup(utils::Point To, int Time);

	protected:
		utils::Point2D from;
		utils::Point2D to;
		utils::Point2D current;
		utils::Point2D speed;

		virtual bool isFinished();
		virtual void Process(int Time);
	};
} }
