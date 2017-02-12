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
			
			if(prev != target.ToMask)
				target.NewMask();

            target.SetDrawMode(target.ToMask);
            mask.DrawIn(target, r);
			
			if(prev != target.ToMask)
				target.SetDrawMode(target.UseMask);

            base.DrawIn(target, r, color);
            target.SetDrawMode(prev);
        }

		void MaskedObject::drawin(TextureTarget &target, const SizeController &controller, const Geometry::Rectanglef &r, RGBAf color) const {
			auto prev = target.GetDrawMode();

			if(prev != target.ToMask)
				target.NewMask();

			target.SetDrawMode(target.ToMask);
			mask.DrawIn(target, controller, r);

			if(prev != target.ToMask)
				target.SetDrawMode(target.UseMask);

			base.DrawIn(target, controller, r, color);
			target.SetDrawMode(prev);
		}

        
        
		void MaskedObject::draw(TextureTarget &target, const Geometry::Pointf &p1, const Geometry::Pointf &p2, 
								const Geometry::Pointf &p3, const Geometry::Pointf &p4, 
								const Geometry::Pointf &tex1, const Geometry::Pointf &tex2, 
								const Geometry::Pointf &tex3, const Geometry::Pointf &tex4, RGBAf color) const 
		{
			auto prev = target.GetDrawMode();

			if(prev != target.ToMask)
				target.NewMask();

			target.SetDrawMode(target.ToMask);
			mask.Draw(target, p1, p2, p3, p4, tex1, tex2, tex3, tex4);

			if(prev != target.ToMask)
				target.SetDrawMode(target.UseMask);

			base.Draw(target, p1, p2, p3, p4, tex1, tex2, tex3, tex4, color);
			target.SetDrawMode(prev);
		}


		void MaskedObject::draw(TextureTarget &target, const Geometry::Pointf &p1, const Geometry::Pointf &p2, const Geometry::Pointf &p3, const Geometry::Pointf &p4, RGBAf color) const {
			auto prev = target.GetDrawMode();

			if(prev != target.ToMask)
				target.NewMask();

			target.SetDrawMode(target.ToMask);
			mask.Draw(target, p1, p2, p3, p4);

			if(prev != target.ToMask)
				target.SetDrawMode(target.UseMask);

			base.Draw(target, p1, p2, p3, p4, color);
			target.SetDrawMode(prev);
		}


		Geometry::Size MaskedObject::getsize() const {
			return base.GetSize();
		}

}
}
