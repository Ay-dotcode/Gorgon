#pragma once

#include "Base.h"
#include "../Graphics/Animations.h"

namespace Gorgon { namespace Resource {

	/// This class denotes the resource is an image animation storage. Unlike graphics based storages,
	/// this interface only allows a rectangular animation storage to be moved out.
	class AnimationStorage : public virtual Base {
	public:
		Graphics::RectangularAnimationStorage MoveOut() {
			return animmoveout();
		}

	protected:
		virtual Graphics::RectangularAnimationStorage animmoveout() = 0;
	};


}}
