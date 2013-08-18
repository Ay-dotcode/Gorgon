#pragma once
#include "../../Resource/Base.h"
#include "../../Engine/Animation.h"
#include "../../Resource/ResizableObject.h"
#include "../Definitions.h"
#include "../../Utils/Margins.h"
#include "../../Resource/File.h"

namespace gge { namespace widgets {

	class RegionResource;

	RegionResource *LoadRegionResource(resource::File& File, std::istream &Data, int Size);


	class Region : public resource::ResizableObject {
	public:
		Region(RegionResource &parent, animation::Timer &controller, bool owner=false);
		Region(RegionResource &parent, bool create=false);

		RegionResource &parent;

		utils::Margins BorderWidth() {
			return utils::Margins(l->GetWidth(), t->GetHeight(), r->GetWidth(), b->GetHeight());
		}

		virtual ~Region() { 
			tl->DeleteAnimation();  
			t->DeleteAnimation();
			tr->DeleteAnimation();
			l->DeleteAnimation();
			c->DeleteAnimation();
			r->DeleteAnimation();
			bl->DeleteAnimation();
			b->DeleteAnimation();
			br->DeleteAnimation();
			tli->DeleteAnimation();  
			tri->DeleteAnimation();
			bli->DeleteAnimation();  
			bri->DeleteAnimation();
		}

		virtual void SetController( animation::Timer &controller, bool owner=false ) {
			Base::SetController(controller, owner);
			tl->SetController(controller);
			t ->SetController(controller);
			tr->SetController(controller);
			l ->SetController(controller);
			c ->SetController(controller);
			r ->SetController(controller);
			bl->SetController(controller);
			b ->SetController(controller);
			br->SetController(controller);
			tli->SetController(controller);
			tri->SetController(controller);
			bli->SetController(controller);
			bri->SetController(controller);
		}

	protected:
		virtual animation::ProgressResult::Type Progress();

		virtual void drawin(graphics::ImageTarget2D& Target, int X, int Y, int W, int H) const; 

		virtual void drawin(graphics::ImageTarget2D& Target, const graphics::SizeController2D &controller, int X, int Y, int W, int H) const;

		virtual int calculatewidth (int w=-1) const;
		virtual int calculateheight(int h=-1) const;

		virtual int calculatewidth (const graphics::SizeController2D &controller, int w=-1) const;
		virtual int calculateheight(const graphics::SizeController2D &controller, int h=-1) const;

		animation::RectangularGraphic2DAnimation 
			*tl, *t , *tr, 
			*l , *c , *r , 
			*bl, *b , *br,
			*tli  ,   *tri,
			*bli  ,   *bri
			;
	};

	class MaskedRegion : public Region {
	public:
		MaskedRegion(RegionResource &parent, animation::Timer &controller, RegionResource *mask, bool owner=false);
		MaskedRegion(RegionResource &parent, RegionResource *mask, bool create=false);

		Region *Mask;

		virtual ~MaskedRegion() {
			delete Mask;
		}

	protected:
		virtual void drawin(graphics::ImageTarget2D& Target, int X, int Y, int W, int H) const;

		virtual void drawin(graphics::ImageTarget2D& Target, const graphics::SizeController2D &controller, int X, int Y, int W, int H) const;
	};

	class RegionResource : public resource::Base, virtual public resource::ResizableObjectProvider, 
		virtual public animation::DiscreteProvider 
	{
		friend RegionResource *LoadRegionResource(resource::File& File, std::istream &Data, int Size);
	public:

		struct TilingInfo {
			TilingInfo() : Top(true), Left(true), Right(true), Bottom(true), Center_Horizontal(true), Center_Vertical(true) 
			{ }
			bool Top				: 1;
			bool Left				: 1;
			bool Right				: 1;
			bool Bottom				: 1;
			bool Center_Horizontal	: 1;
			bool Center_Vertical	: 1;
		};

		RegionResource(
			animation::RectangularGraphic2DSequenceProvider &tl, animation::RectangularGraphic2DSequenceProvider &t, animation::RectangularGraphic2DSequenceProvider &tr, 
			animation::RectangularGraphic2DSequenceProvider &l , animation::RectangularGraphic2DSequenceProvider &c, animation::RectangularGraphic2DSequenceProvider &r , 
			animation::RectangularGraphic2DSequenceProvider &bl, animation::RectangularGraphic2DSequenceProvider &b, animation::RectangularGraphic2DSequenceProvider &br, 
			animation::RectangularGraphic2DSequenceProvider &tli,                                                    animation::RectangularGraphic2DSequenceProvider &tri,
			animation::RectangularGraphic2DSequenceProvider &bli,                                                    animation::RectangularGraphic2DSequenceProvider &bri,
			TilingInfo Tiling
			) : 
		Tiling(Tiling),
			tl(&tl), t (&t ), tr(&tr),
			l ( &l), c (&c ), r ( &r),
			bl(&bl), b (&b ), br(&br),
			tli(&tli),        tri(&tri),
			bli(&bli),        bri(&bri),
			Mask(NULL)
		{ }

		virtual GID::Type GetGID() const { return GID::Region; }

		virtual Region &CreateAnimation(animation::Timer &controller, bool owner=false) {
			return CreateResizableObject(controller, owner);
		}
		virtual Region &CreateAnimation(bool create=false) {
			return CreateResizableObject(create);
		}

		virtual Region &CreateResizableObject(animation::Timer &controller, bool owner=false) { 
			if(Mask==NULL)
				return *new Region(*this, controller,owner); 
			else
				return *new MaskedRegion(*this, controller, Mask,owner); 
		}
		virtual Region &CreateResizableObject(bool create=false) { 
			if(Mask==NULL)
				return *new Region(*this, create); 
			else
				return *new MaskedRegion(*this, Mask, create); 
		}

		const animation::RectangularGraphic2DSequenceProvider &GetTL() const { return *tl; }
		animation::RectangularGraphic2DSequenceProvider &GetTL() { return *tl; }

		const animation::RectangularGraphic2DSequenceProvider &GetT() const { return *t; }
		animation::RectangularGraphic2DSequenceProvider &GetT() { return *t; }

		const animation::RectangularGraphic2DSequenceProvider &GetTR() const { return *tr; }
		animation::RectangularGraphic2DSequenceProvider &GetTR() { return *tr; }

		const animation::RectangularGraphic2DSequenceProvider &GetL() const { return *l; }
		animation::RectangularGraphic2DSequenceProvider &GetL() { return *l; }

		const animation::RectangularGraphic2DSequenceProvider &GetC() const { return *c; }
		animation::RectangularGraphic2DSequenceProvider &GetC() { return *c; }

		const animation::RectangularGraphic2DSequenceProvider &GetR() const { return *r; }
		animation::RectangularGraphic2DSequenceProvider &GetR() { return *r; }

		const animation::RectangularGraphic2DSequenceProvider &GetBL() const { return *bl; }
		animation::RectangularGraphic2DSequenceProvider &GetBL() { return *bl; }

		const animation::RectangularGraphic2DSequenceProvider &GetB() const { return *b; }
		animation::RectangularGraphic2DSequenceProvider &GetB() { return *b; }

		const animation::RectangularGraphic2DSequenceProvider &GetBR() const { return *br; }
		animation::RectangularGraphic2DSequenceProvider &GetBR() { return *br; }

		const animation::RectangularGraphic2DSequenceProvider &GetTLI() const { return *tli; }
		animation::RectangularGraphic2DSequenceProvider &GetTLI() { return *tli; }

		const animation::RectangularGraphic2DSequenceProvider &GetTRI() const { return *tri; }
		animation::RectangularGraphic2DSequenceProvider &GetTRI() { return *tri; }

		const animation::RectangularGraphic2DSequenceProvider &GetBLI() const { return *bli; }
		animation::RectangularGraphic2DSequenceProvider &GetBLI() { return *bli; }

		const animation::RectangularGraphic2DSequenceProvider &GetBRI() const { return *bri; }
		animation::RectangularGraphic2DSequenceProvider &GetBRI() { return *bri; }

		virtual void SetSources(
			animation::RectangularGraphic2DSequenceProvider &tl, animation::RectangularGraphic2DSequenceProvider &t, animation::RectangularGraphic2DSequenceProvider &tr, 
			animation::RectangularGraphic2DSequenceProvider &l , animation::RectangularGraphic2DSequenceProvider &c, animation::RectangularGraphic2DSequenceProvider &r , 
			animation::RectangularGraphic2DSequenceProvider &bl, animation::RectangularGraphic2DSequenceProvider &b, animation::RectangularGraphic2DSequenceProvider &br,
			animation::RectangularGraphic2DSequenceProvider &tli,                                                    animation::RectangularGraphic2DSequenceProvider &tri,
			animation::RectangularGraphic2DSequenceProvider &bli,                                                    animation::RectangularGraphic2DSequenceProvider &bri
			) {
				this->tl=&tl; this->t=&t; this->tr=&tr;
				this-> l=& l; this->c=&c; this-> r=& r;
				this->bl=&bl; this->b=&b; this->br=&br;
				this->tli=&tli;           this->tri=&tri;
				this->bli=&bli;           this->bri=&bri;
		}

		utils::Margins BorderWidth() {
			return utils::Margins(GetL().ImageAt(0).GetWidth(), GetT().ImageAt(0).GetHeight(), GetR().ImageAt(0).GetWidth(), GetB().ImageAt(0).GetHeight());
		}

		TilingInfo Tiling;

		virtual int GetDuration() const	 { return c->GetDuration(); }
		virtual int GetDuration(unsigned Frame) const { return c->GetDuration(Frame); }
		virtual int GetNumberofFrames() const { return c->GetNumberofFrames(); }

		virtual int		 FrameAt(unsigned Time) const { return c->FrameAt(Time); }
		virtual int		 StartOf(unsigned Frame) const { return c->StartOf(Frame); }
		virtual	int		 EndOf(unsigned Frame) const { return c->EndOf(Frame); }

		virtual void Prepare(GGEMain &main, resource::File &file) {
			Base::Prepare(main, file);
			Mask=dynamic_cast<RegionResource*>(file.Root().FindObject(mask));
		}

		RegionResource *Mask;
	protected:
		RegionResource() : 
			Tiling(),
			tl(NULL), t (NULL), tr(NULL),
			l (NULL), c (NULL), r (NULL),
			bl(NULL), b (NULL), br(NULL),
			tli(NULL)    ,      tri(NULL),
			bli(NULL)    ,      bri(NULL),
			Mask(NULL)
		{ }

		animation::RectangularGraphic2DSequenceProvider 
			*tl, *t , *tr,
			*l , *c , *r ,
			*bl, *b , *br,
			*tli  ,   *tri,
			*bli  ,   *bri
			;

		utils::SGuid mask;
	};

	inline int Region::calculatewidth (int w) const {
		return w; 
	}
	inline int Region::calculateheight(int h) const { 
		return h; 
	}
	inline int Region::calculatewidth (const graphics::SizeController2D &controller, int w) const  { 
		return controller.CalculateWidth(w, c->GetWidth(), l->GetWidth()+r->GetWidth());
	}
	inline int Region::calculateheight (const graphics::SizeController2D &controller, int h) const  { 
		return controller.CalculateHeight(h, c->GetHeight(), t->GetHeight()+b->GetHeight());
	}

	inline Region::Region(RegionResource &parent, animation::Timer &controller, bool owner/*=false*/) : parent(parent), Base(controller, owner) {
		tl=&parent.GetTL().CreateAnimation(controller);
		t=&parent.GetT().CreateAnimation(controller);
		tr=&parent.GetTR().CreateAnimation(controller);
		l=&parent.GetL().CreateAnimation(controller);
		c=&parent.GetC().CreateAnimation(controller);
		r=&parent.GetR().CreateAnimation(controller);
		bl=&parent.GetBL().CreateAnimation(controller);
		b=&parent.GetB().CreateAnimation(controller);
		br=&parent.GetBR().CreateAnimation(controller);
		tli=&parent.GetTL().CreateAnimation(controller);
		tri=&parent.GetTR().CreateAnimation(controller);
		bli=&parent.GetTL().CreateAnimation(controller);
		bri=&parent.GetTR().CreateAnimation(controller);
	}

	inline Region::Region(RegionResource &parent, bool create/*=false*/) : parent(parent), Base(create) {
		if(Controller) {
			tl=&parent.GetTL().CreateAnimation(*Controller);
			t=&parent.GetT().CreateAnimation(*Controller);
			tr=&parent.GetTR().CreateAnimation(*Controller);
			l=&parent.GetL().CreateAnimation(*Controller);
			c=&parent.GetC().CreateAnimation(*Controller);
			r=&parent.GetR().CreateAnimation(*Controller);
			bl=&parent.GetBL().CreateAnimation(*Controller);
			b=&parent.GetB().CreateAnimation(*Controller);
			br=&parent.GetBR().CreateAnimation(*Controller);
			tli=&parent.GetTL().CreateAnimation(*Controller);
			tri=&parent.GetTR().CreateAnimation(*Controller);
			bli=&parent.GetTL().CreateAnimation(*Controller);
			bri=&parent.GetTR().CreateAnimation(*Controller);
		}
		else {
			tl=&parent.GetTL().CreateAnimation();
			t=&parent.GetT().CreateAnimation();
			tr=&parent.GetTR().CreateAnimation();
			l=&parent.GetL().CreateAnimation();
			c=&parent.GetC().CreateAnimation();
			r=&parent.GetR().CreateAnimation();
			bl=&parent.GetBL().CreateAnimation();
			b=&parent.GetB().CreateAnimation();
			br=&parent.GetBR().CreateAnimation();
			tli=&parent.GetTL().CreateAnimation();
			tri=&parent.GetTR().CreateAnimation();
			bli=&parent.GetTL().CreateAnimation();
			bri=&parent.GetTR().CreateAnimation();
		}
	}


	inline MaskedRegion::MaskedRegion(RegionResource &parent, animation::Timer &controller, RegionResource *mask, bool owner/*=false*/) : 
	Region(parent, controller, owner) {
		Mask=&mask->CreateResizableObject(controller);
	}

	inline MaskedRegion::MaskedRegion(RegionResource &parent, RegionResource *mask, bool create/*=false*/) : 
	Region(parent, create) {
		if(Controller)
			Mask=&mask->CreateResizableObject(*Controller);
		else
			Mask=&mask->CreateResizableObject();
	}



}}