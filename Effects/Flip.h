#pragma once

#include "../Resource/GRE.h"
#include "../Engine/Graphics.h"
#include "../Engine/GraphicLayers.h"
#include "../Engine/Graphic2D.h"
#include "../Engine/Animation.h"

namespace gge { namespace effects {

	class FlipEffect : public animation::Base, public graphics::Graphic2D {
	public:
		enum FlipSide {
			Top=1,
			Left,
			Bottom,
			Right,
		};

		FlipEffect(animation::Timer &controller, bool owner=false) : Base(controller,owner) { 
			Initialize(); 
		}

		explicit FlipEffect(bool create=false) : Base(create) { 
			Initialize(); 
		}

		void Flip(int ETA);
		void CenterPivot();

		graphics::RectangularGraphic2D *Front,*Back;
		FlipSide Side;
		bool Backside;
		bool Flipping;
		float Perspective;
		int ETA;

		utils::Point PivotFront;
		utils::Point PivotBack;

	protected:
		void Initialize();

		virtual animation::ProgressResult::Type Progress();

		virtual void draw(graphics::ImageTarget2D& Target, int X, int Y);

		void SetProgress(int progress) {
			if(Controller)
				Controller->SetProgress(progress);
		}

		int GetProgress() {
			if(Controller)
				return Controller->GetProgress();
			else
				return 0;
		}

		void Reset() {
			if(Controller)
				Controller->ResetProgress();
		}
	};
} }
