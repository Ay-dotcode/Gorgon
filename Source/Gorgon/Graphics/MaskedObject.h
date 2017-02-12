#pragma once

#include "EmptyImage.h"
#include "Animations.h"

namespace Gorgon { namespace Graphics {
    
    class MaskedObjectProvider;
    
    class MaskedObject : public virtual RectangularAnimation {
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

		virtual void draw(TextureTarget &target, const Geometry::Pointf &p1, const Geometry::Pointf &p2, 
						  const Geometry::Pointf &p3, const Geometry::Pointf &p4, 
						  const Geometry::Pointf &tex1, const Geometry::Pointf &tex2, 
						  const Geometry::Pointf &tex3, const Geometry::Pointf &tex4, RGBAf color) const override;


		virtual void draw(TextureTarget &target, const Geometry::Pointf &p1, const Geometry::Pointf &p2, 
						  const Geometry::Pointf &p3, const Geometry::Pointf &p4, RGBAf color) const override;


		virtual Geometry::Size getsize() const override;

	private:
        const MaskedObjectProvider &parent;
		RectangularAnimation &base, &mask;
    };

    /**
     * This object creates a masked object from two rectangular animations. Static images
     * can be used as single frame animations. While drawing, color only affects the base
	 * image.
     */
    class MaskedObjectProvider : public RectangularAnimationProvider {
    public:
        MaskedObjectProvider(RectangularAnimationProvider &base, RectangularAnimationProvider &mask) :
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
		RectangularAnimation &CreateBase() const {
            if(base) {
                return base->CreateAnimation(false);
            }
            else {
                return EmptyImage::Instance();
            }
        }
        
        /// Creates a mask animation without controller.
		RectangularAnimation &CreateMask() const {
            if(mask) {
                return mask->CreateAnimation(false);
            }
            else {
                return EmptyImage::Instance();
            }
        }
        
        /// Sets the providers in this object
        void SetProviders(RectangularAnimationProvider &base, RectangularAnimationProvider &mask) {
            this->base = &base;
            this->mask = &mask;
        }
        
    private:
		RectangularAnimationProvider *base;
		RectangularAnimationProvider *mask;
    };

    
} }
