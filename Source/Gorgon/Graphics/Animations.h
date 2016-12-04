#pragma once

#pragma warning(disable:4250)

#include "Drawables.h"
#include "../Animation.h"
#include "../Animation/Storage.h"

namespace Gorgon { namespace Graphics { 

	class Animation : public virtual Drawable, public virtual Gorgon::Animation::Base {

	};

	class SizelessAnimation : public virtual SizelessDrawable, public virtual Gorgon::Animation::Base {

	};

	class RectangularAnimation : public virtual RectangularDrawable, public virtual SizelessAnimation, public virtual Animation {

	};

	class AnimationProvider : public virtual Gorgon::Animation::Provider {
	public:
		using AnimationType = Animation;

		/// This function should create a new animation with the given controller and
		/// if owner parameter is set to true, it should assume ownership of the controller
		virtual Animation &CreateAnimation(Gorgon::Animation::Timer &timer) const override = 0;

		/// This function should create and animation and depending on the create parameter,
		/// it should create its own timer.
		virtual Animation &CreateAnimation(bool create=true) const override = 0;
	};

	class SizelessAnimationProvider : public virtual Gorgon::Animation::Provider {
	public:
		using AnimationType = SizelessAnimation;

		/// This function should create a new animation with the given controller and
		/// if owner parameter is set to true, it should assume ownership of the controller
		virtual SizelessAnimation &CreateAnimation(Gorgon::Animation::Timer &timer) const override = 0;

		/// This function should create and animation and depending on the create parameter,
		/// it should create its own timer.
		virtual SizelessAnimation &CreateAnimation(bool create=true) const override = 0;
	};

	class RectangularAnimationProvider : public virtual AnimationProvider {
	public:
		using AnimationType = RectangularAnimation;

		/// This function should create a new animation with the given controller and
		/// if owner parameter is set to true, it should assume ownership of the controller
		virtual RectangularAnimation &CreateAnimation(Gorgon::Animation::Timer &timer) const override = 0;

		/// This function should create and animation and depending on the create parameter,
		/// it should create its own timer.
		virtual RectangularAnimation &CreateAnimation(bool create=true) const override = 0;
	};

	using AnimationStorage = Gorgon::Animation::basic_Storage<AnimationProvider>;
	using SizelessAnimationStorage = Gorgon::Animation::basic_Storage<SizelessAnimationProvider>;
	using RectangularAnimationStorage = Gorgon::Animation::basic_Storage<RectangularAnimationProvider>;

} }
