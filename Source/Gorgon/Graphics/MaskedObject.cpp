#include "MaskedObject.h"


namespace Gorgon { namespace Graphics {

        MaskedObject::MaskedObject(const MaskedObjectProvider &parent, bool create) : 
            Gorgon::Animation::Base(create), parent(parent),
            base(parent.CreateBase()), mask(parent.CreateMask())
        {
            if(HasController()) {
                base.SetController(GetController());
                mask.SetController(GetController());
            }
        }
        
        MaskedObject::MaskedObject(const MaskedObjectProvider &parent, Gorgon::Animation::ControllerBase &timer) : 
            Gorgon::Animation::Base(timer), parent(parent),
            base(parent.CreateBase()), mask(parent.CreateMask())
        {
            if(HasController()) {
                base.SetController(GetController());
                mask.SetController(GetController());
            }
        }
        
        void MaskedObject::drawin(TextureTarget &target, const Geometry::Rectanglef &r, RGBAf color) const {
            auto prev = target.GetDrawMode();
            target.NewMask();
            target.SetDrawMode(target.ToMask);
            mask.DrawIn(target, r);
            target.SetDrawMode(target.UseMask);
            base.DrawIn(target, r, color);
            target.SetDrawMode(prev);
        }

		void MaskedObject::drawin(TextureTarget &target, const SizeController &controller, const Geometry::Rectanglef &r, RGBAf color) const {
            auto prev = target.GetDrawMode();
            target.NewMask();
            target.SetDrawMode(target.ToMask);
            mask.DrawIn(target, controller, r);
            target.SetDrawMode(target.UseMask);
            base.DrawIn(target, controller, r, color);
            target.SetDrawMode(prev);
        }

        
        
} }
