#pragma once

#include "../Engine/Graphic2D.h"
#include "../Engine/Animation.h"

namespace gge { namespace resource {

	class File;

	class ResizableObject : public graphics::SizelessGraphic2D, public virtual animation::AnimationBase  {
	public:
		ResizableObject() : AnimationBase() { }
		ResizableObject(animation::AnimationTimer &Controller) : AnimationBase(Controller) { }

		
	};

	class ResizableObjectProvider : public animation::AnimationProvider {
	public:
		virtual ResizableObject &CreateAnimation(animation::AnimationTimer &controller, bool owner=false) = 0;
		virtual ResizableObject &CreateAnimation(bool create=false) = 0;
	};

} }
