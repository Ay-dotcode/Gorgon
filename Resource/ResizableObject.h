#pragma once

#include "../Engine/Graphic2D.h"
#include "../Engine/Animation.h"

namespace gge { namespace resource {

	class File;

	class ResizableObject : public virtual graphics::SizelessGraphic2D, public virtual animation::AnimationBase  {
	public:

		
	};

	class ResizableObjectProvider : virtual public animation::AnimationProvider {
	public:
		virtual ResizableObject &CreateResizableObject(animation::AnimationTimer &controller, bool owner=false) = 0;
		virtual ResizableObject &CreateResizableObject(bool create=false) = 0;
	};

} }
