#pragma once

#pragma warning(disable:4250)

#include "Drawables.h"
#include "../Animation.h"
#include "../Animation/Storage.h"

namespace Gorgon { namespace Graphics { 

	class Animation : public virtual Drawable, public virtual Gorgon::Animation::Base {

	};
	
	class RectangularAnimation : public virtual RectangularDrawable, public virtual Animation {

	};

	class AnimationProvider : public virtual Gorgon::Animation::Provider {
	public:
		using AnimationType = Animation;
        
        /// This function moves this animation provider into a new provider. Ownership of this new object belongs
        /// to the caller and this object could be destroyed safely.
        virtual AnimationProvider &MoveOutProvider() override = 0;

		/// This function should create a new animation with the given controller and
		/// if owner parameter is set to true, it should assume ownership of the controller
		virtual Animation &CreateAnimation(Gorgon::Animation::ControllerBase &timer) const override = 0;

		/// This function should create and animation and depending on the create parameter,
		/// it should create its own timer.
		virtual Animation &CreateAnimation(bool create=true) const override = 0;
	};

	class RectangularAnimationProvider : public AnimationProvider {
	public:
		using AnimationType = RectangularAnimation;
        
        /// This function moves this animation provider into a new provider. Ownership of this new object belongs
        /// to the caller and this object could be destroyed safely.
        virtual RectangularAnimationProvider &MoveOutProvider() override = 0;

		/// This function should create a new animation with the given controller and
		/// if owner parameter is set to true, it should assume ownership of the controller
		virtual RectangularAnimation &CreateAnimation(Gorgon::Animation::ControllerBase &timer) const override = 0;

		/// This function should create and animation and depending on the create parameter,
		/// it should create its own timer.
		virtual RectangularAnimation &CreateAnimation(bool create=true) const override = 0;
	};

	using AnimationStorage = Gorgon::Animation::basic_Storage<AnimationProvider>;
	using RectangularAnimationStorage = Gorgon::Animation::basic_Storage<RectangularAnimationProvider>;

} }
