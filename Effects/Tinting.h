#pragma once

#include "../Resource/GRE.h"
#include "../Engine/Animation.h"
#include "../Engine/Graphics.h"
#include "../Engine/GraphicLayers.h"

namespace gge { namespace effects {

	////This effect tints a given colorizable target. Tinting is animated from a given
	/// value to another one
	class Tinting : public animation::Base {
	public:

		////Initializes the effect
		Tinting(graphics::Colorizable2DLayer *Target, animation::Timer &Controller, bool owner=false) : 
			Base(Controller, owner),
			from(0), to(0), current(0), 
			speed(0, 0,0,0),
			Target(Target)
		{ } 

		////Initializes the effect
		Tinting(graphics::Colorizable2DLayer &Target, animation::Timer &Controller, bool owner=false) : 
			Base(Controller, owner),
			from(0), to(0), current(0), 
			speed(0, 0,0,0),
			Target(&Target)
		{ }

		////Initializes the effect
		Tinting(graphics::Colorizable2DLayer *Target, bool create=false); 

		////Initializes the effect
		Tinting(graphics::Colorizable2DLayer &Target, bool create=false);
		
		////Sets source and destination to the given values and allows time duration to reach the
		/// destination
		void Setup(graphics::RGBint From, graphics::RGBint To, int Time);
		////Sets current destination to the given value and allows time duration to reach it
		void Setup(graphics::RGBint To, int Time) { Setup(current, To, Time); }

		bool IsFinished() { return from == to; }

		void SetTarget(graphics::Colorizable2DLayer *target) { Target=target; Progress(); }
		void SetTarget(graphics::Colorizable2DLayer &target) { Target=&target; Progress(); }
		graphics::Colorizable2DLayer &GetTarget() { return *Target; }
	protected:
		////Target of this effect
		graphics::Colorizable2DLayer *Target;

		graphics::RGBfloat from;
		graphics::RGBfloat to;
		graphics::RGBfloat current;
		graphics::RGBfloat speed;

		virtual animation::ProgressResult::Type Progress();
	};
} }
