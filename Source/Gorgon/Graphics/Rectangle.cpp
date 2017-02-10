#include "Rectangle.h"

namespace Gorgon { namespace Graphics {

	Rectangle::Rectangle(const IRectangleProvider &prov, bool create /*= true*/) : Gorgon::Animation::Base(create), prov(prov),
		tl(prov.CreateTL()),
		tm(prov.CreateTM()),
		tr(prov.CreateTR()),
		ml(prov.CreateML()),
		mm(prov.CreateMM()),
		mr(prov.CreateMR()),
		bl(prov.CreateBL()),
		bm(prov.CreateBM()),
		br(prov.CreateBR())
    {
		if(HasController()) {
			tl.SetController(GetController());
			tm.SetController(GetController());
			tr.SetController(GetController());
			ml.SetController(GetController());
			mm.SetController(GetController());
			mr.SetController(GetController());
			bl.SetController(GetController());
			bm.SetController(GetController());
			br.SetController(GetController());
		}
	}

	Rectangle::Rectangle(const IRectangleProvider &prov, Gorgon::Animation::ControllerBase &timer) : Gorgon::Animation::Base(timer), prov(prov),
		tl(prov.CreateTL()),
		tm(prov.CreateTM()),
		tr(prov.CreateTR()),
		ml(prov.CreateML()),
		mm(prov.CreateMM()),
		mr(prov.CreateMR()),
		bl(prov.CreateBL()),
		bm(prov.CreateBM()),
		br(prov.CreateBR())
    {
		if(HasController()) {
			tl.SetController(GetController());
			tm.SetController(GetController());
			tr.SetController(GetController());
			ml.SetController(GetController());
			mm.SetController(GetController());
			mr.SetController(GetController());
			bl.SetController(GetController());
			bm.SetController(GetController());
			br.SetController(GetController());
		}
	}

	void Rectangle::drawin(TextureTarget &target, const Geometry::Rectanglef &r, RGBAf color) const {
        int maxt = std::max(std::max(tl.GetHeight(), tm.GetHeight()), tr.GetHeight());
        int maxb = std::max(std::max(bl.GetHeight(), bm.GetHeight()), br.GetHeight());
        
        int maxl = std::max(std::max(tl.GetWidth(), ml.GetWidth()), bl.GetWidth());
        int maxr = std::max(std::max(tr.GetWidth(), mr.GetWidth()), br.GetWidth());
        
        tl.Draw(target, r.X + maxl - tl.GetWidth(), r.Y + maxt - tl.GetHeight(), color);
        tm.DrawIn(target, prov.GetTiling() ? Tiling::Horizontal : Tiling::None, 
                  Geometry::Rectanglef(r.X + maxl, r.Y + maxt - tm.GetHeight(), r.Width-maxl-maxr, (Float)tm.GetHeight()),
                  color);
        tr.Draw(target, r.Right() - maxr, r.Y + maxt - tr.GetHeight());

        ml.DrawIn(target, prov.GetTiling() ? Tiling::Vertical : Tiling::None, 
                  Geometry::Rectanglef(r.X + maxl - ml.GetWidth(), r.Y + maxt, (Float)ml.GetWidth(), r.Height-maxt-maxb),
                  color);        
        mm.DrawIn(target, prov.GetTiling() ? Tiling::Both : Tiling::None, 
                  Geometry::Rectanglef(r.X + maxl, r.Y + maxt, r.Width-maxl-maxr, r.Height-maxt-maxb),
                  color);
        mr.DrawIn(target, prov.GetTiling() ? Tiling::Vertical : Tiling::None, 
                  Geometry::Rectanglef(r.Right() - maxr, r.Y + maxt, (Float)ml.GetWidth(), r.Height-maxt-maxb),
                  color);
        
        
        bl.Draw(target, r.X + maxl - bl.GetWidth(), r.Bottom() - maxb, color);
        bm.DrawIn(target, prov.GetTiling() ? Tiling::Horizontal : Tiling::None, 
                  Geometry::Rectanglef(r.X + maxl, r.Bottom() - maxb, r.Width-maxl-maxr, (Float)bm.GetHeight()),
                  color);
        br.Draw(target, r.Right() - maxr, r.Bottom() - maxb);
	}

	void Rectangle::drawin(TextureTarget &target, const SizeController &controller, const Geometry::Rectanglef &r, RGBAf color) const {
		SizeController c = controller;

		if(!prov.GetTiling()) {
			c.Horizontal =  c.Stretch;
			c.Vertical =  c.Stretch;
		}

		float maxt = (float)std::max(std::max(tl.GetHeight(), tm.GetHeight()), tr.GetHeight());
		float maxb = (float)std::max(std::max(bl.GetHeight(), bm.GetHeight()), br.GetHeight());

		float maxl = (float)std::max(std::max(tl.GetWidth(), ml.GetWidth()), bl.GetWidth());
		float maxr = (float)std::max(std::max(tr.GetWidth(), mr.GetWidth()), br.GetWidth());

		auto newr = c.CalculateArea(Geometry::Sizef(mm.GetSize()), {maxl+maxr, maxt+maxb}, r.GetSize());
		newr = newr + r.TopLeft();

		drawin(target, newr, color);
	}
} }
