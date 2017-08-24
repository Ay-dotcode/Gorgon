#pragma once

#pragma warning(disable:4250)

#include "Drawables.h"
#include "../Animation.h"
#include "../Animation/Storage.h"

namespace Gorgon { namespace Graphics { 

	/// A regular drawable animation
	class Animation : public virtual Drawable, public virtual Gorgon::Animation::Base {

	};
	
	/// Rectangular drawable animation
	class RectangularAnimation : public virtual RectangularDrawable, public virtual Animation {

	};

	/// A discrete rectangular animation, this is most suitable for bitmap or texture animations
	class DiscreteAnimation: public virtual Graphics::RectangularAnimation, public virtual Gorgon::Animation::DiscreteAnimation {
	};

	/// A regular drawable animation provider
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

	/// This class provides rectangular animations
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

	/// This class provides discrete and rectangular animation which is suitable for bitmap and texture animations
	class DiscreteAnimationProvider : public virtual Graphics::RectangularAnimationProvider, public virtual Gorgon::Animation::DiscreteProvider {
	};

	using AnimationStorage = Gorgon::Animation::basic_Storage<AnimationProvider>;
	using RectangularAnimationStorage = Gorgon::Animation::basic_Storage<RectangularAnimationProvider>;

} }
