#pragma once

#include "GRE.h"
#include "Base.h"
#include "../Engine/Animation.h"
#include "Image.h"
#include "../Resource/ResizableObject.h"
#include "../Utils/Point2D.h"
#include "Animation.h"
#include "../Engine/Pointer.h"

#pragma warning(push)
#pragma warning(disable:4250)

namespace gge { namespace resource {

	class File;
	class Pointer;

	Pointer *LoadPointerResource(File &File, std::istream &Data, int Size);

	class Pointer : public Base, virtual public ResizableObjectProvider, virtual public animation::RectangularGraphic2DSequenceProvider {
		friend Pointer *LoadPointerResource(File &file, std::istream &Data, int Size);
	public:
		Pointer() : animation(new Animation), Hotspot(0,0), Type(gge::Pointer::Arrow) { }
		virtual ~Pointer() { utils::CheckAndDelete(animation); }

		utils::Point Hotspot;
		gge::Pointer::PointerType Type;

		virtual ImageAnimation &CreateAnimation(animation::Timer &controller, bool owner=false) {
			return animation->CreateResizableObject(controller, owner);
		}

		virtual ImageAnimation &CreateAnimation(bool create=false) {
			return animation->CreateResizableObject(create);
		}

		virtual ImageAnimation &CreateResizableObject(animation::Timer &controller, bool owner=false) {
			return animation->CreateResizableObject(controller, owner);
		}

		virtual ImageAnimation &CreateResizableObject(bool create=false) {
			return animation->CreateResizableObject(create);
		}

		virtual Image &ImageAt(int t) { 
			return animation->ImageAt(t); 
		}

		virtual int FrameAt(unsigned t) const {
			return animation->FrameAt(t);
		}

		virtual int StartOf(unsigned Frame) const {
			return animation->StartOf(Frame);
		}

		virtual int GetDuration(unsigned Frame) const {
			return animation->GetDuration(Frame);
		}
		virtual int GetDuration() const {
			return animation->GetDuration();
		} 

		virtual int GetNumberofFrames() const {
			return animation->GetNumberofFrames();
		}

		GID::Type GetGID() const {
			return GID::Pointer;
		}

		virtual void Prepare(GGEMain &main, File &file) { animation->Prepare(main, file); }

	protected:
		Animation *animation;

	};

}}
