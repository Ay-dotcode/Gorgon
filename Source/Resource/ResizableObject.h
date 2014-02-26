#pragma once

#include "../Engine/Graphic2D.h"
#include "../Engine/Animation.h"

namespace gge { namespace resource {

	class File;

	class ResizableObject : public virtual graphics::SizelessGraphic2D, public virtual animation::Base  {
	public:

		virtual ~ResizableObject() {}
	};

	class ResizableObjectProvider : virtual public animation::Provider {
	public:
		virtual ResizableObject &CreateResizableObject(animation::Timer &controller, bool owner=false) = 0;
		virtual ResizableObject &CreateResizableObject(bool create=false) = 0;
	};

} }
