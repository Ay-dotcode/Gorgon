#pragma once

#include "../Resource/GRE.h"
#include "../Engine/Animator.h"
#include "../Engine/Graphics.h"
#include "../Engine/GraphicLayers.h"

namespace gge { namespace effects {

	class FlipEffect : public AnimatorBase, public graphics::Buffered2DGraphic {
	public:
		enum FlipSide {
			Top=1,
			Left,
			Bottom,
			Right,
		};
		////This event is fired when the animation
		/// completes
		utils::EventChain<FlipEffect> FinishedEvent;

		FlipEffect() : FinishedEvent("Finished", this) { 
			AnimatorBase::FinishedEvent.DoubleLink(FinishedEvent);
			Initialize(); 
		}

		void Flip(int ETA);
		void CenterPivot();

		graphics::Buffered2DGraphic *Front,*Back;
		FlipSide Side;
		bool Backside;
		bool Flipping;
		float Perspective;
		int ETA;

		utils::Point PivotFront;
		utils::Point PivotBack;

		virtual void Draw(graphics::I2DGraphicsTarget *Layer, int X, int Y);
		virtual void Draw(graphics::I2DGraphicsTarget &Layer, int X, int Y) { Draw(&Layer, X,Y); }

	protected:
		void Initialize();

		virtual bool isFinished() { return AnimatorBase::currentTime()>=ETA; }
		virtual void Process(int Time);
	};
} }
