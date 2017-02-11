#pragma once

#include "EmptyImage.h"
#include "Animations.h"

namespace Gorgon { namespace Graphics {
    
    class MaskedObjectProvider;
    
    class MaskedObject : public virtual SizelessAnimation {
    public:
        MaskedObject(const MaskedObjectProvider &parent, bool create = true);
        
        MaskedObject(const MaskedObjectProvider &parent, Gorgon::Animation::ControllerBase &timer);
        
        ~MaskedObject() {
            base.DeleteAnimation();
            mask.DeleteAnimation();
        }

		virtual bool Progress(unsigned &) override {
			return true; //individual parts will work automatically
		}
        
    protected:
		virtual void drawin(TextureTarget &target, const Geometry::Rectanglef &r, RGBAf color) const override;

		virtual void drawin(TextureTarget &target, const SizeController &controller, const Geometry::Rectanglef &r, RGBAf color) const override;

		virtual Geometry::Size calculatesize(const Geometry::Size &area) const override {
            return Union(base.CalculateSize(area), mask.CalculateSize(area));
		}

		virtual Geometry::Size calculatesize(const SizeController &controller, const Geometry::Size &s) const override {
            return Union(base.CalculateSize(controller, s), mask.CalculateSize(controller, s));
		}        
		
    private:
        const MaskedObjectProvider &parent;
        SizelessAnimation &base, &mask;
    };

    /**
     * This object creates a masked object from two sizeless animations. Rectangular animations
     * can also act like sizeless animations as they can be tiled. Additionally, static images
     * can be used as single frame animations. Thus this class can cover most cases.
     */
    class MaskedObjectProvider : public SizelessAnimationProvider {
    public:
        MaskedObjectProvider(SizelessAnimationProvider &base, SizelessAnimationProvider &mask) : 
            base(&base), mask(&mask) 
        { }
		
        MaskedObjectProvider(MaskedObjectProvider &&other) : 
            base(other.base), mask(other.mask) 
        { 
            other.base = nullptr;
            other.mask = nullptr;
        }
 
        //types are derived not to type the same code for every class
		virtual auto MoveOutProvider() -> decltype(*this) override {
            auto ret = new typename std::remove_reference<decltype(*this)>::type(std::move(*this));
            
            return *ret;
        }
       
        MaskedObject &CreateAnimation(bool create = true) const override {
            return *new MaskedObject(*this, create);
        }
        
        MaskedObject &CreateAnimation(Gorgon::Animation::ControllerBase &timer) const override {
            return *new MaskedObject(*this, timer);
        }
        
        /// Creates a base animation without controller.
        SizelessAnimation &CreateBase() const {
            if(base) {
                return dynamic_cast<SizelessAnimation&>(base->CreateAnimation(false));
            }
            else {
                return EmptyImage::Instance();
            }
        }
        
        /// Creates a mask animation without controller.
        SizelessAnimation &CreateMask() const {
            if(mask) {
                return dynamic_cast<SizelessAnimation&>(mask->CreateAnimation(false));
            }
            else {
                return EmptyImage::Instance();
            }
        }
        
        /// Sets the providers in this object
        void SetProviders(SizelessAnimationProvider &base, SizelessAnimationProvider &mask) {
            this->base = &base;
            this->mask = &mask;
        }
        
    private:
        SizelessAnimationProvider *base;
        SizelessAnimationProvider *mask;
    };

    
} }
