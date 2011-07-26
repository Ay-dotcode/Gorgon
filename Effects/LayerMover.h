#pragma once

#include "../Resource/GRE.h"
#include "../Engine/Animation.h"
#include "../Engine/Graphics.h"
#include "../Engine/Layer.h"

namespace gge { namespace effects {

	////This effect moves a given layer. Moving operation is animated from a given
	/// value to another one
	class LayerMover : public animation::AnimationBase {
	public:

		////Target of this effect
		LayerBase *Target;

		////Initializes the effect
		LayerMover(LayerBase *Target, animation::AnimationTimer &controller, bool owner=false);
		////Initializes the effect
		LayerMover(LayerBase &Target, animation::AnimationTimer &controller, bool owner=false);

		////Initializes the effect
		LayerMover(LayerBase *Target, bool create=false);
		////Initializes the effect
		LayerMover(LayerBase &Target, bool create=false);

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
		virtual animation::ProgressResult::Type Progress();
	};
} }
