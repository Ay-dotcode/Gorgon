#pragma once

#include "../Utils/GGE.h"
#include "../Resource/GRE.h"
#include "../Engine/Animator.h"
#include "../Engine/Graphics.h"
#include "../Engine/GraphicLayers.h"

namespace geffects {
	enum FlipSide {
		EAFS_Top=1,
		EAFS_Left,
		EAFS_Bottom,
		EAFS_Right,
	};

	class FlipEffect : public gge::AnimatorBase, public gge::Buffered2DGraphic {
	public:
		////This event is fired when the animation
		/// completes
		utils::EventChain<FlipEffect> FinishedEvent;

		FlipEffect() : FinishedEvent("Finished", this) { 
			AnimatorBase::FinishedEvent.DoubleLink(FinishedEvent);
			Initialize(); 
		}

		void Flip(int ETA);
		void CenterPivot();

		gge::Buffered2DGraphic *Front,*Back;
		FlipSide Side;
		bool Backside;
		bool Flipping;
		float Perspective;
		int ETA;

		Point PivotFront;
		Point PivotBack;

		virtual void Draw(I2DGraphicsTarget *Layer, int X, int Y);
		virtual void Draw(I2DGraphicsTarget &Layer, int X, int Y) { Draw(&Layer, X,Y); }

	protected:
		void Initialize();

		virtual bool isFinished() { return AnimatorBase::currentTime()>=ETA; }
		virtual void Process(int Time);
	};
};