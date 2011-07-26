#pragma once

#include "../Resource/GRE.h"
#include "../Engine/Animation.h"
#include "../Engine/Graphics.h"
#include "../Engine/Layer.h"
#include "../Utils/Rectangle2D.h"

namespace gge { namespace effects {

	////This effect resizes a given layer. Resize operation is animated from a given
	/// value to another one
	class LayerResizer : public animation::AnimationBase {
	public:

		////Target of this effect
		LayerBase *Target;

		////Initializes the effect
		LayerResizer(LayerBase *Target, animation::AnimationTimer &controller, bool owner=false) :
			AnimationBase(controller,owner),
			speed(0,0 , 0,0),
			current((utils::Bounds2D)Target->BoundingBox),
			Target(Target)
		{
		}

		////Initializes the effect
		LayerResizer(LayerBase &Target, animation::AnimationTimer &controller, bool owner=false) :
			AnimationBase(controller,owner),
			speed(0,0 , 0,0),
			current((utils::Bounds2D)Target.BoundingBox),
			Target(&Target)
		{
		}

			////Initializes the effect
		LayerResizer(LayerBase *Target, bool create=false) :
			AnimationBase(create),
			speed(0,0 , 0,0),
			current((utils::Bounds2D)Target->BoundingBox),
			Target(Target)
		{
		}

		////Initializes the effect
		LayerResizer(LayerBase &Target, bool create=false) :
			AnimationBase(create),
			speed(0,0 , 0,0),
			current((utils::Bounds2D)Target.BoundingBox),
			Target(&Target)
		{
		}
		////Sets source and destination to the given values and allows time duration to reach the
		/// destination
		void Setup(utils::Rectangle From, utils::Rectangle To, int Time);
		////Sets current destination to the given value and allows time duration to reach it
		void Setup(utils::Rectangle To, int Time);

	protected:
		utils::Rectangle2D from;
		utils::Rectangle2D to;
		utils::Rectangle2D current;
		utils::Rectangle2D speed;

		virtual animation::ProgressResult::Type Progress();
	};
} }
