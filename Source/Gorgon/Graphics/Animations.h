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

		virtual Geometry::Size GetSize() const = 0;

		int GetWidth() const { return GetSize().Width; }
		int GetHeight() const { return GetSize().Height; }
	};

	/// This class provides discrete and rectangular animation which is suitable for bitmap and texture animations
	class DiscreteAnimationProvider : public virtual Graphics::RectangularAnimationProvider, public virtual Gorgon::Animation::DiscreteProvider {
    public:
 		using AnimationType = DiscreteAnimation;

		/// This function should create a new animation with the given controller and
		/// if owner parameter is set to true, it should assume ownership of the controller
		virtual DiscreteAnimation &CreateAnimation(Gorgon::Animation::ControllerBase &timer) const override = 0;

		/// This function should create and animation and depending on the create parameter,
		/// it should create its own timer.
		virtual DiscreteAnimation &CreateAnimation(bool create=true) const override = 0;

	};
    
} 

namespace Animation { 
    
    /// Injects additional functionality for AnimationProviders
    template<>
    class basic_StorageInjection<Graphics::AnimationProvider> {
        //nothing for now
    };
    
    /// Injects additional functionality for RectangularAnimationProviders
    template<>
    class basic_StorageInjection<Graphics::RectangularAnimationProvider> : public Graphics::RectangularAnimationProvider {
        
        //types are derived not to type the same code for every class
		virtual auto MoveOutProvider() -> decltype(*this) override {
            auto ret = new typename std::remove_reference<decltype(*this)>::type(std::move(*this));
            
            return *ret;
        }

		virtual Graphics::RectangularAnimation &CreateAnimation(Gorgon::Animation::ControllerBase &timer) const override {
            auto &me = dynamic_cast<const basic_Storage<Graphics::RectangularAnimationProvider>&>(*this);
            return me.CreateAnimation(timer);
        }

		virtual Graphics::RectangularAnimation &CreateAnimation(bool create=true) const override {
            auto &me = dynamic_cast<const basic_Storage<Graphics::RectangularAnimationProvider>&>(*this);
            return me.CreateAnimation(create);
        }

		virtual Geometry::Size GetSize() const override {
            auto &me = dynamic_cast<const basic_Storage<Graphics::RectangularAnimationProvider>&>(*this);
            if(me.HasAnimation())
                return me.GetAnimation().GetSize();
            else
                return {0, 0};
        }
        
		int GetWidth() const { return GetSize().Width; }
		int GetHeight() const { return GetSize().Height; }
    };
    
    /// Injects additional functionality for RectangularAnimationProviders
    template<>
    class basic_StorageInjection<Graphics::DiscreteAnimationProvider> : public Graphics::DiscreteAnimationProvider {
    public:
        //types are derived not to type the same code for every class
		virtual auto MoveOutProvider() -> decltype(*this) override {
            auto ret = new typename std::remove_reference<decltype(*this)>::type(std::move(*this));
            
            return *ret;
        }

		virtual Graphics::DiscreteAnimation &CreateAnimation(Gorgon::Animation::ControllerBase &timer) const override {
            auto &me = dynamic_cast<const basic_Storage<Graphics::DiscreteAnimationProvider>&>(*this);
            return me.CreateAnimation(timer);
        }

		virtual Graphics::DiscreteAnimation &CreateAnimation(bool create=true) const override {
            auto &me = dynamic_cast<const basic_Storage<Graphics::DiscreteAnimationProvider>&>(*this);
            return me.CreateAnimation(create);
        }
        
		virtual Geometry::Size GetSize() const override {
            auto &me = dynamic_cast<const basic_Storage<Graphics::DiscreteAnimationProvider>&>(*this);
            if(me.HasAnimation())
                return me.GetAnimation().GetSize();
            else
                return {0, 0};
        }
        
        
        virtual void Add(const Frame &frame) override {
            auto &me = dynamic_cast<basic_Storage<Graphics::DiscreteAnimationProvider>&>(*this);
            if(me.HasAnimation())
                me.GetAnimation().Add(frame);
		}

        virtual void Insert(const Frame &frame, int before) override {
            auto &me = dynamic_cast<basic_Storage<Graphics::DiscreteAnimationProvider>&>(*this);
            if(me.HasAnimation())
                me.GetAnimation().Insert(frame, before);
		}

        virtual void MoveBefore(unsigned index, int before) override {
            auto &me = dynamic_cast<basic_Storage<Graphics::DiscreteAnimationProvider>&>(*this);
            if(me.HasAnimation())
			    me.GetAnimation().MoveBefore(index, before);
		}

        virtual void Remove(unsigned index) override {
            auto &me = dynamic_cast<basic_Storage<Graphics::DiscreteAnimationProvider>&>(*this);
            if(me.HasAnimation())
			    me.GetAnimation().Remove(index);
		}

        virtual const Frame &FrameAt(int index) const override {
            auto &me = dynamic_cast<const basic_Storage<Graphics::DiscreteAnimationProvider>&>(*this);
            if(me.HasAnimation())
			    return me.GetAnimation().FrameAt(index);
            else
                throw std::runtime_error("Animation storage is empty");
		}

        virtual unsigned StartOf(unsigned frame) const override {
            auto &me = dynamic_cast<const basic_Storage<Graphics::DiscreteAnimationProvider>&>(*this);
            if(me.HasAnimation())
			    return me.GetAnimation().StartOf(frame);
            else
                return -1;
		}

        virtual unsigned GetDuration() const override {
            auto &me = dynamic_cast<const basic_Storage<Graphics::DiscreteAnimationProvider>&>(*this);
            if(me.HasAnimation())
			    return me.GetAnimation().GetDuration();
            else
                return 0;
		}

        virtual unsigned GetDuration(unsigned frame) const override {
            auto &me = dynamic_cast<const basic_Storage<Graphics::DiscreteAnimationProvider>&>(*this);
            if(me.HasAnimation())
                return me.GetAnimation().GetDuration(frame);
            else
                return 0;
		}

        virtual void Clear() override {
            auto &me = dynamic_cast<basic_Storage<Graphics::DiscreteAnimationProvider>&>(*this);
            if(me.HasAnimation())
                me.GetAnimation().Clear();
		}

        virtual int GetCount() const override {
            auto &me = dynamic_cast<const basic_Storage<Graphics::DiscreteAnimationProvider>&>(*this);
            if(me.HasAnimation())
                return me.GetAnimation().GetCount();
            else
                return 0;
		}


		int GetWidth() const { return GetSize().Width; }
		int GetHeight() const { return GetSize().Height; }
    };
    
} 

namespace Graphics { 

	using AnimationStorage = Gorgon::Animation::basic_Storage<AnimationProvider>;
	using RectangularAnimationStorage = Gorgon::Animation::basic_Storage<RectangularAnimationProvider>;
	using DiscreteAnimationStorage = Gorgon::Animation::basic_Storage<DiscreteAnimationProvider>;

} }
