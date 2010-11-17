#pragma once

#include "../Utils/GGE.h"
#include "../Resource/GRE.h"
#include "../Engine/Animator.h"
#include "../Engine/Graphics.h"
#include "../Engine/GraphicLayers.h"

using namespace gre;

namespace geffects {
	enum FlipSide {
		EAFS_Top=1,
		EAFS_Left,
		EAFS_Bottom,
		EAFS_Right,
	};

	class FlipEffect : public AnimatorBase, public Buffered2DGraphic {
	public:
		////This event is fired when the animation
		/// completes
		EventChain<FlipEffect, empty_event_params> FinishedEvent;

		FlipEffect() : FinishedEvent("Finished", this) { 
			AnimatorBase::FinishedEvent=FinishedEvent;
			Initialize(); 
		}

		void Flip(int ETA);
		void CenterPivot();

		Buffered2DGraphic *Front,*Back;
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