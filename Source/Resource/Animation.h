#pragma once

#include "GRE.h"
#include "Base.h"
#include "../Engine/Animation.h"
#include "Image.h"
#include "../Resource/ResizableObject.h"
#include "../Utils/Point2D.h"

#pragma warning(push)
#pragma warning(disable:4250)

namespace gge { namespace resource {
	class File;

	class Animation;
	
	////This function loads a text resource from the given file
	Animation *LoadAnimationResource(File &File, std::istream &Data, int Size);

	////This class draws an animated image
	class ImageAnimation : 
		virtual public ResizableObject, public graphics::ImageTexture, virtual public animation::RectangularGraphic2DAnimation
	{
	public:

		ImageAnimation(Animation &parent, animation::Timer &controller, bool owner=false);
		ImageAnimation(Animation &parent, bool create=false);

		virtual graphics::GLTexture &GetTexture() { return ImageTexture::GetTexture(); }

		Animation &parent;

		virtual void DeleteAnimation() { 
			ImageTexture::Texture.ID=0;

			delete this; 
		}

	protected:
		virtual animation::ProgressResult::Type Progress();
	};


	class AnimationFrame {
	public:
		AnimationFrame(unsigned d=0, unsigned s=0, Image *im=NULL) : Duration(d), Start(s), Image(im) { }

		unsigned Duration;
		unsigned Start;
		resource::Image *Image;
	};


	////
	class Animation : 
		public Base, virtual public ResizableObjectProvider, 
		virtual public animation::RectangularGraphic2DSequenceProvider 
	{

		friend Animation *LoadAnimationResource(File &File, std::istream &Data, int Size);
		friend void LoadAnimationResourceEx(Animation *anim, File &File, std::istream &Data, int Size);
		friend class ImageAnimation;
		friend class DiscreteImageAnimation;
	public:
		////03010000h (Gaming, Animation)
		virtual GID::Type GetGID() const { return GID::Animation; }
		
		////Default constructor
		Animation() : Base() { FrameCount=TotalLength=0; }

		////Returns the width of the first image
		int GetWidth() const { if(Frames.size()>0) return Frames[0].Image->GetWidth(); return 0; }
		////Returns the height of the first image
		int GetHeight() const { if(Frames.size()>0) return Frames[0].Image->GetHeight(); return 0; }
		////Returns number of frames
		int GetFrameCount() const { return FrameCount; }

		virtual ImageAnimation &CreateAnimation(animation::Timer &controller, bool owner=false) {
			return *new ImageAnimation(*this, controller, owner);
		}

		virtual ImageAnimation &CreateAnimation(bool create=false) {
			return *new ImageAnimation(*this, create);
		}

		virtual ImageAnimation &CreateResizableObject(animation::Timer &controller, bool owner=false) {
			return *new ImageAnimation(*this, controller, owner);
		}

		virtual ImageAnimation &CreateResizableObject(bool create=false) {
			return *new ImageAnimation(*this, create);
		}

		virtual Image &ImageAt(int t) { 
			t=utils::PositiveMod(t, GetDuration());

			return *Frames[FrameAt(t)].Image; 
		} //Null Image
		
		//graphics::RectangularGraphic2D &GraphicAt(unsigned t) { return *Frames[FrameAt(t)].Image; } //Null Image

		unsigned GetTotalLength() const { return TotalLength; }

		Image &operator [](int Frame) {
			return *Frames[Frame].Image;
		}

		virtual int FrameAt(unsigned t) const;

		virtual int StartOf(unsigned Frame) const {
			return Frames[Frame].Start;
		}

		virtual int GetDuration(unsigned Frame) const {
			return Frames[Frame].Duration;
		}
		virtual int GetDuration() const {
			return TotalLength;
		} 

		virtual int GetNumberofFrames() const {
			return Frames.size();
		}

		std::function<void(File&,std::istream&,GID::Type,int)> loadextra;

	protected:
		////Total number of frames that this animation have
		int FrameCount;
		////Frame durations
		std::vector<AnimationFrame> Frames;
		unsigned TotalLength;

	};
} }

#pragma warning(pop)