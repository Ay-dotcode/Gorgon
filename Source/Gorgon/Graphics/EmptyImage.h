#include "Animations.h"

namespace Gorgon { namespace Graphics {

	/**
	 * This class is an empty image that will not draw anything if drawn on a layer.
	 * Its size is always 0x0 and it satisfies the requirements for animation. Only
	 * one EmptyImage is enough but this is not enforced. This class is separated into
	 * two due to a bug in Visual Studio
	 */
	class EmptyImage : public virtual RectangularAnimation, public RectangularAnimationProvider 
	{
	public:
		/// If guard is set in debug, EmptyImage is guarded against deletion.
		explicit EmptyImage(bool guard = false) : 
#ifndef NDEBUG
			guard(guard)
#endif
		{
		}

		virtual ~EmptyImage() {
#ifndef NDEBUG
			ASSERT(!guard, "EmptyImage is destroyed.");
#endif
		}

		void DeleteAnimation() const override { }

		virtual bool Progress(unsigned &leftover) override {
			return true;
		}

		virtual EmptyImage &CreateAnimation(Gorgon::Animation::Timer &timer) const override {
			return const_cast<EmptyImage&>(*this);
		}


		virtual EmptyImage &CreateAnimation(bool create=true) const override {
			return const_cast<EmptyImage&>(*this);
		}

		/// Returns the instance for empty image. Only one instance is enough.
		static EmptyImage &Instance() { static EmptyImage me(true); return me; }

	protected:
		virtual void drawin(TextureTarget &target, const SizeController &controller, const Geometry::Rectanglef &r, RGBAf color) const override final {
		}


		virtual Geometry::Size calculatesize(const Geometry::Size &area) const override final {
			return{0,0};
		}


		virtual Geometry::Size calculatesize(const SizeController &controller, const Geometry::Size &s) const override final {
			return {0,0};
		}


		virtual void draw(TextureTarget &target, const Geometry::Pointf &p1, const Geometry::Pointf &p2, const Geometry::Pointf &p3, const Geometry::Pointf &p4, const Geometry::Pointf &tex1, const Geometry::Pointf &tex2, const Geometry::Pointf &tex3, const Geometry::Pointf &tex4, RGBAf color) const override final {
		}


		virtual void draw(TextureTarget &target, const Geometry::Pointf &p1, const Geometry::Pointf &p2, const Geometry::Pointf &p3, const Geometry::Pointf &p4, RGBAf color) const override final {
		}


		virtual Geometry::Size getsize() const override final {
			return {0, 0};
		}

	private:
#ifndef NDEBUG
		bool guard;
#endif
	};

	/**
	* This class is an empty image that will not draw anything if drawn on a layer.
	* Its size is always 0x0 and it satisfies the requirements for animation. Only
	* one EmptyImage is enough but this is not enforced. This class is separated into
	* two due to a bug in Visual Studio
	*/
	class EmptySizelessImage : public virtual SizelessAnimation, public SizelessAnimationProvider {
	public:
		/// If guard is set in debug, EmptyImage is guarded against deletion.
		explicit EmptySizelessImage(bool guard = false) :
#ifndef NDEBUG
			guard(guard)
#endif
		{}

		virtual ~EmptySizelessImage() {
#ifndef NDEBUG
			ASSERT(!guard, "EmptyImage is destroyed.");
#endif
		}

		void DeleteAnimation() const override {}

		virtual bool Progress(unsigned &leftover) override {
			return true;
		}

		virtual EmptySizelessImage &CreateAnimation(Gorgon::Animation::Timer &timer) const override {
			return const_cast<EmptySizelessImage&>(*this);
		}


		virtual EmptySizelessImage &CreateAnimation(bool create=true) const override {
			return const_cast<EmptySizelessImage&>(*this);
		}

		/// Returns the instance for empty image. Only one instance is enough.
		static EmptySizelessImage &Instance() { static EmptySizelessImage me(true); return me; }

	protected:
		virtual void drawin(TextureTarget &target, const SizeController &controller, const Geometry::Rectanglef &r, RGBAf color) const override final {}


		virtual Geometry::Size calculatesize(const Geometry::Size &area) const override final {}


		virtual Geometry::Size calculatesize(const SizeController &controller, const Geometry::Size &s) const override final {}


		virtual void drawin(TextureTarget &target, const Geometry::Rectanglef &r, RGBAf color) const override {
		}

	private:
#ifndef NDEBUG
		bool guard;
#endif
	};

}}