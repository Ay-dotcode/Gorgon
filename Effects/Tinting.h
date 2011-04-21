#pragma once

#include "../Utils/GGE.h"
#include "../Resource/GRE.h"
#include "../Engine/Animator.h"
#include "../Engine/Graphics.h"

namespace gge { namespace effects {

	////This effect tints a given colorizable target. Tinting is animated from a given
	/// value to another one
	class Tinting : public AnimatorBase {
	public:
		////This event is fired when the animation
		/// completes
		utils::EventChain<Tinting> FinishedEvent;

		////Target of this effect
		graphics::I2DColorizableGraphicsTarget *Target;

		////Initializes the effect
		Tinting(graphics::I2DColorizableGraphicsTarget *Target) : 
			from(), to(), current(), 
			speed(0, 0,0,0),
			Target(Target),
			FinishedEvent("Finished", this)
		{
			AnimatorBase::FinishedEvent.DoubleLink(FinishedEvent);
		}

		////Initializes the effect
		Tinting(graphics::I2DColorizableGraphicsTarget &Target) : 
			from(), to(), current(), 
			speed(0, 0,0,0),
			Target(&Target),
			FinishedEvent("Finished", this)
		{
			AnimatorBase::FinishedEvent.DoubleLink(FinishedEvent);
		}
		
		////Sets source and destination to the given values and allows time duration to reach the
		/// destination
		void Setup(graphics::RGBint From, graphics::RGBint To, int Time);
		////Sets current destination to the given value and allows time duration to reach it
		void Setup(graphics::RGBint To, int Time) { Setup(current, To, Time); }

	protected:
		graphics::RGBfloat from;
		graphics::RGBfloat to;
		graphics::RGBfloat current;
		graphics::RGBfloat speed;

		virtual bool isFinished();
		virtual void Process(int Time);
	};
} }
