#pragma once

#include "Base.h"
#include "../Graphics/Animations.h"

namespace Gorgon { namespace Resource {

	/// This class denotes the resource is an image animation storage. Unlike graphics based storages,
	/// this interface only allows a rectangular animation storage to be moved out.
	class RectangularAnimationStorage : public Base {
	public:
		Graphics::RectangularAnimationStorage MoveOut() {
			return animmoveout();
		}

	protected:
		virtual Graphics::RectangularAnimationStorage animmoveout() = 0;
	};

	/// This class denotes the resource is a sizeless animation storage. Unlike graphics based storages,
	/// this interface only allows a sizeless animation storage to be moved out.
	class SizelessAnimationStorage : public Base {
    public:
		Graphics::SizelessAnimationStorage MoveOut() {
			return animmoveout();
		}

	protected:
		virtual Graphics::SizelessAnimationStorage animmoveout() = 0;
	};

}}
