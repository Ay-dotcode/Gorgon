#pragma once
#include "../../Resource/Base.h"
#include "../../Engine/Animation.h"
#include "../../Resource/ResizableObject.h"
#include "../Definitions.h"
#include "../../Utils/Margins.h"
#include "../../Resource/File.h"

namespace gge { namespace widgets {

	class RectangleResource;

	RectangleResource *LoadRectangleResource(resource::File& File, std::istream &Data, int Size);

	class MaskedRectangle;

	class Rectangle : public resource::ResizableObject {
		friend class MaskedRectangle;
	public:
		Rectangle(RectangleResource &parent, animation::Timer &controller, bool owner=false);
		Rectangle(RectangleResource &parent, bool create=false);

		RectangleResource &parent;

		utils::Margins BorderWidth() {
			return utils::Margins(l->GetWidth(), t->GetHeight(), r->GetWidth(), b->GetHeight());
		}

		virtual ~Rectangle() { 
			tl->DeleteAnimation();  
			t->DeleteAnimation();
			tr->DeleteAnimation();
			l->DeleteAnimation();
			c->DeleteAnimation();
			r->DeleteAnimation();
			bl->DeleteAnimation();
			b->DeleteAnimation();
			br->DeleteAnimation();
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
			*bl, *b , *br
		;
	};

	class MaskedRectangle : public Rectangle {
	public:
		MaskedRectangle(RectangleResource &parent, animation::Timer &controller, RectangleResource *mask, bool owner=false);
		MaskedRectangle(RectangleResource &parent, RectangleResource *mask, bool create=false);

		Rectangle *Mask;

		virtual ~MaskedRectangle() {
			delete Mask;
		}

	protected:
		virtual void drawin(graphics::ImageTarget2D& Target, int X, int Y, int W, int H) const;

		virtual void drawin(graphics::ImageTarget2D& Target, const graphics::SizeController2D &controller, int X, int Y, int W, int H) const;
	};

	class RectangleResource : public resource::Base, virtual public resource::ResizableObjectProvider, 
		virtual public animation::DiscreteProvider 
	{
		friend RectangleResource *LoadRectangleResource(resource::File& File, std::istream &Data, int Size);
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

		RectangleResource(
			animation::RectangularGraphic2DSequenceProvider &tl, animation::RectangularGraphic2DSequenceProvider &t, animation::RectangularGraphic2DSequenceProvider &tr, 
			animation::RectangularGraphic2DSequenceProvider &l , animation::RectangularGraphic2DSequenceProvider &c, animation::RectangularGraphic2DSequenceProvider &r , 
			animation::RectangularGraphic2DSequenceProvider &bl, animation::RectangularGraphic2DSequenceProvider &b, animation::RectangularGraphic2DSequenceProvider &br, 
			TilingInfo Tiling
		) : 
			Tiling(Tiling),
			tl(&tl), t (&t ), tr(&tr),
			l ( &l), c (&c ), r ( &r),
			bl(&bl), b (&b ), br(&br),
			Mask(NULL)
		{ }

		virtual GID::Type GetGID() const { return GID::Rectangle; }

		virtual Rectangle &CreateAnimation(animation::Timer &controller, bool owner=false) {
			return CreateResizableObject(controller, owner);
		}
		virtual Rectangle &CreateAnimation(bool create=false) {
			return CreateResizableObject(create);
		}

		virtual Rectangle &CreateResizableObject(animation::Timer &controller, bool owner=false) { 
			if(Mask==NULL)
				return *new Rectangle(*this, controller,owner); 
			else
				return *new MaskedRectangle(*this, controller, Mask,owner); 
		}
		virtual Rectangle &CreateResizableObject(bool create=false) { 
			if(Mask==NULL)
				return *new Rectangle(*this, create); 
			else
				return *new MaskedRectangle(*this, Mask, create); 
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

		virtual void SetSources(
			animation::RectangularGraphic2DSequenceProvider &tl, animation::RectangularGraphic2DSequenceProvider &t, animation::RectangularGraphic2DSequenceProvider &tr, 
			animation::RectangularGraphic2DSequenceProvider &l , animation::RectangularGraphic2DSequenceProvider &c, animation::RectangularGraphic2DSequenceProvider &r , 
			animation::RectangularGraphic2DSequenceProvider &bl, animation::RectangularGraphic2DSequenceProvider &b, animation::RectangularGraphic2DSequenceProvider &br
		) {
			this->tl=&tl; this->t=&t; this->tr=&tr;
			this-> l=& l; this->c=&c; this-> r=& r;
			this->bl=&bl; this->b=&b; this->br=&br;
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
			Mask=dynamic_cast<RectangleResource*>(file.Root().FindObject(mask));
		}

		RectangleResource *Mask;
	protected:
		RectangleResource() : 
		Tiling(),
			tl(NULL), t (NULL), tr(NULL),
			l (NULL), c (NULL), r (NULL),
			bl(NULL), b (NULL), br(NULL),
			Mask(NULL)
		{ }

		animation::RectangularGraphic2DSequenceProvider 
			*tl, *t , *tr,
			*l , *c , *r ,
			*bl, *b , *br
		;

		utils::SGuid mask;
	};

	inline int Rectangle::calculatewidth (int w) const {
		return w; 
	}
	inline int Rectangle::calculateheight(int h) const { 
		return h; 
	}
	inline int Rectangle::calculatewidth (const graphics::SizeController2D &controller, int w) const  { 
		return controller.CalculateWidth(w, c->GetWidth(), l->GetWidth()+r->GetWidth());
	}
	inline int Rectangle::calculateheight (const graphics::SizeController2D &controller, int h) const  { 
		return controller.CalculateHeight(h, c->GetHeight(), t->GetHeight()+b->GetHeight());
	}

	inline Rectangle::Rectangle(RectangleResource &parent, animation::Timer &controller, bool owner/*=false*/) : parent(parent), Base(controller, owner) {
		tl=&parent.GetTL().CreateAnimation(controller);
		t=&parent.GetT().CreateAnimation(controller);
		tr=&parent.GetTR().CreateAnimation(controller);
		l=&parent.GetL().CreateAnimation(controller);
		c=&parent.GetC().CreateAnimation(controller);
		r=&parent.GetR().CreateAnimation(controller);
		bl=&parent.GetBL().CreateAnimation(controller);
		b=&parent.GetB().CreateAnimation(controller);
		br=&parent.GetBR().CreateAnimation(controller);
	}

	inline Rectangle::Rectangle(RectangleResource &parent, bool create/*=false*/) : parent(parent), Base(create) {
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
		}
	}


	inline MaskedRectangle::MaskedRectangle(RectangleResource &parent, animation::Timer &controller, RectangleResource *mask, bool owner/*=false*/) : 
	Rectangle(parent, controller, owner) {
		Mask=&mask->CreateResizableObject(controller);
	}

	inline MaskedRectangle::MaskedRectangle(RectangleResource &parent, RectangleResource *mask, bool create/*=false*/) : 
	Rectangle(parent, create) {
		if(Controller)
			Mask=&mask->CreateResizableObject(*Controller);
		else
			Mask=&mask->CreateResizableObject();
	}



}}