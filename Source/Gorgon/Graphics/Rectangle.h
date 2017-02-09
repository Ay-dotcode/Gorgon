#pragma once

#include "Animations.h"
#include "ImageAnimation.h"
#include "EmptyImage.h"
#include "Bitmap.h"

namespace Gorgon { namespace Graphics {
	/// Interface for RectangleProviders
	class IRectangleProvider : public SizelessAnimationProvider {
	public:
		IRectangleProvider() {}

		/// Creates an animation without controller. This function should always return an animation
		virtual RectangularAnimation &CreateTL() const = 0;

		/// Creates an animation without controller. This function should always return an animation
		virtual RectangularAnimation &CreateTM() const = 0;

		/// Creates an animation without controller. This function should always return an animation
		virtual RectangularAnimation &CreateTR() const = 0;

		/// Creates an animation without controller. This function should always return an animation
		virtual RectangularAnimation &CreateML() const = 0;

		/// Creates an animation without controller. This function should always return an animation
		virtual RectangularAnimation &CreateMM() const = 0;

		/// Creates an animation without controller. This function should always return an animation
		virtual RectangularAnimation &CreateMR() const = 0;

		/// Creates an animation without controller. This function should always return an animation
		virtual RectangularAnimation &CreateBL() const = 0;

		/// Creates an animation without controller. This function should always return an animation
		virtual RectangularAnimation &CreateBM() const = 0;

		/// Creates an animation without controller. This function should always return an animation
		virtual RectangularAnimation &CreateBR() const = 0;

		/// Sets whether the middle parts would be tiled. If set to false it will be stretched to fit the
		/// given area. Instances will require redrawing before this change is reflected. This effects the
		/// entire system. Tiling is recommended for all applications.
		virtual void SetTiling(bool value) {
			tiling = value;
		}

		/// Returns if the middle part will be tiled.
		virtual bool GetTiling() const {
			return tiling;
		}

	private:
		bool tiling = true;
	};

	/**
	* This class allows drawing a rectangle like image that is made out of nine parts. Rectangles can be scaled
	* freely without loss of quality.
	* See basic_RectangleProvider for details.
	*/
	class Rectangle : public SizelessAnimation {
	public:
		Rectangle(const IRectangleProvider &prov, Gorgon::Animation::ControllerBase &timer);

		Rectangle(const IRectangleProvider &prov, bool create = true);

		virtual ~Rectangle() {
			tl.DeleteAnimation();
			tm.DeleteAnimation();
			tr.DeleteAnimation();
			ml.DeleteAnimation();
			mm.DeleteAnimation();
			mr.DeleteAnimation();
			bl.DeleteAnimation();
			bm.DeleteAnimation();
			br.DeleteAnimation();
		}

		virtual bool Progress(unsigned &) override {
			return true; //individual parts will work automatically
		}

	protected:
		virtual void drawin(TextureTarget &target, const Geometry::Rectanglef &r, RGBAf color) const override;

		virtual void drawin(TextureTarget &target, const SizeController &controller, const Geometry::Rectanglef &r, RGBAf color) const override;

		virtual Geometry::Size calculatesize(const Geometry::Size &area) const override {
			return area;
		}

		virtual Geometry::Size calculatesize(const SizeController &controller, const Geometry::Size &s) const override {
			return controller.CalculateSize({mm.GetSize()}, {ml.GetWidth()+mr.GetWidth(), tm.GetHeight()+bm.GetHeight()}, s);
		}

	private:
		RectangularAnimation &tl, &tm, &tr;
		RectangularAnimation &ml, &mm, &mr;
		RectangularAnimation &bl, &bm, &br;

		const IRectangleProvider &prov;
	};

	/**
	* This class allows instancing of a rectangle like image that is made out of three
	* parts. The first part is the start of the rectangle, the second part is the middle
	* and the third part is the end of the rectangle. Middle part can be repeated or
	* stretched. A rectangle provider can have empty animations. Provider will use
	* EmptyImage for missing parts. A_ must derive from RectangularAnimationProvider.
	* For best results, try to keep size of parts compatible. If an image is smaller, it
	* will be placed towards the center. Part names follows a logic, first letter is
	* vertical location, second is horizontal. For instance TL is top left, ML is directly
	* the left of the middle section. Any part that has an M in it will be scaled/tiled.
	*/
	template<class A_>
	class basic_RectangleProvider : public IRectangleProvider {
	public:
		using AnimationType = Rectangle;

		/// Empty constructor, rectangle can be instanced even if it is completely empty
		basic_RectangleProvider() : IRectangleProvider() {}

		/// Filling constructor
		basic_RectangleProvider(
			A_  &tl, A_ &tm, A_ &tr,
			A_  &ml, A_ &mm, A_ &mr,
			A_  &bl, A_ &bm, A_ &br
		) : 
			tl(&tl), tm(&tm), tr(&tr),
			ml(&ml), mm(&mm), mr(&mr),
			bl(&bl), bm(&bm), br(&br) 
        { }

		/// Filling constructor, nullptr is acceptable
		basic_RectangleProvider(
			A_  *tl, A_ *tm, A_ *tr,
			A_  *ml, A_ *mm, A_ *mr,
			A_  *bl, A_ *bm, A_ *br
		) :
			tl(tl), tm(tm), tr(tr),
			ml(ml), mm(mm), mr(mr),
			bl(bl), bm(bm), br(br) 
        { }

		~basic_RectangleProvider() {
			if(owned) {
				delete tl;
				delete tm;
				delete tr;

                delete ml;
				delete mm;
				delete mr;
    
                delete bl;
				delete bm;
				delete br;
            }
		}

		Rectangle &CreateAnimation(Gorgon::Animation::ControllerBase &timer) const override {
			return *new Rectangle(*this, timer);
		}

		Rectangle &CreateAnimation(bool create = true) const override {
			return *new Rectangle(*this, create);
		}

		virtual RectangularAnimation &CreateTL() const override {
			if(tl)
				return tl->CreateAnimation(false);
			else
				return EmptyImage::Instance();
		}


		virtual RectangularAnimation &CreateTM() const override {
			if(tm)
				return tm->CreateAnimation(false);
			else
				return EmptyImage::Instance();
		}


		virtual RectangularAnimation &CreateTR() const override {
			if(tr)
				return tr->CreateAnimation(false);
			else
				return EmptyImage::Instance();
		}


		virtual RectangularAnimation &CreateML() const override {
			if(ml)
				return ml->CreateAnimation(false);
			else
				return EmptyImage::Instance();
		}


		virtual RectangularAnimation &CreateMM() const override {
			if(mm)
				return mm->CreateAnimation(false);
			else
				return EmptyImage::Instance();
		}


		virtual RectangularAnimation &CreateMR() const override {
			if(mr)
				return mr->CreateAnimation(false);
			else
				return EmptyImage::Instance();
		}


		virtual RectangularAnimation &CreateBL() const override {
			if(bl)
				return bl->CreateAnimation(false);
			else
				return EmptyImage::Instance();
		}


		virtual RectangularAnimation &CreateBM() const override {
			if(bm)
				return bm->CreateAnimation(false);
			else
				return EmptyImage::Instance();
		}


		virtual RectangularAnimation &CreateBR() const override {
			if(br)
				return br->CreateAnimation(false);
			else
				return EmptyImage::Instance();
		}

		/// Returns TL provider, may return nullptr.
		A_ *GetTL() const {
            return tl;
        }

		/// Returns TM provider, may return nullptr.
		A_ *GetTM() const {
            return tm;
        }

		/// Returns TR provider, may return nullptr.
		A_ *GetTR() const {
            return tr;
        }

		/// Returns ML provider, may return nullptr.
		A_ *GetML() const {
            return ml;
        }

		/// Returns MM provider, may return nullptr.
		A_ *GetMM() const {
            return mm;
        }

		/// Returns MR provider, may return nullptr.
		A_ *GetMR() const {
            return mr;
        }

		/// Returns BL provider, may return nullptr.
		A_ *GetBL() const {
            return bl;
        }

		/// Returns BM provider, may return nullptr.
		A_ *GetBM() const {
            return bm;
        }

		/// Returns BR provider, may return nullptr.
		A_ *GetBR() const {
            return br;
        }

		/// Prepares all animation providers if the they support Prepare function.
		void Prepare() {
			if(tl)
				tl->Prepare();
			if(tm)
				tm->Prepare();
			if(tr)
				tr->Prepare();

			if(ml)
				ml->Prepare();
			if(mm)
				mm->Prepare();
			if(mr)
				mr->Prepare();

			if(bl)
				bl->Prepare();
			if(bm)
				bm->Prepare();
			if(br)
				br->Prepare();
		}

		/// Issuing this function will make this rectangle to own its providers
		/// destroying them when they are done.
		void OwnProviders() {
			owned = true;
		}

	private:
		A_ *tl = nullptr;
		A_ *tm = nullptr;
		A_ *tr = nullptr;

		A_ *ml = nullptr;
		A_ *mm = nullptr;
		A_ *mr = nullptr;

		A_ *bl = nullptr;
		A_ *bm = nullptr;
		A_ *br = nullptr;

		bool owned = false;
	};

	using RectangleProvider = basic_RectangleProvider<RectangularAnimationProvider>;
	using BitmapRectangleProvider = basic_RectangleProvider<Bitmap>;
	using AnimatedBitmapRectangleProvider = basic_RectangleProvider<BitmapAnimationProvider>;

} }
