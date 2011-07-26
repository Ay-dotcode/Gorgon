#pragma once

#include "GRE.h"
#include "ResourceBase.h"
#include "../Engine/Animation.h"
#include "ImageResource.h"
#include "../Resource/ResizableObject.h"
#include "../Utils/Point2D.h"

namespace gge { namespace resource {
	class File;
	
	////This function loads a text resource from the given file
	ResourceBase *LoadAnimationResource(File &File, std::istream &Data, int Size);

	class AnimationResource;

	////This class draws an animated image
	class ImageAnimation : 
		public ResizableObject, public virtual graphics::RectangularGraphic2D, 
		protected graphics::TextureImage
	{
	public:

		ImageAnimation(AnimationResource &parent, animation::AnimationTimer &controller, bool owner=false);
		ImageAnimation(AnimationResource &parent, bool create=false);

		AnimationResource &parent;
	protected:
		virtual animation::ProgressResult::Type Progress();

		virtual void drawin(graphics::ImageTarget2D& Target, int X, int Y, int W, int H) 
		{ drawto(Target, graphics::Tiling2D::Both, X,Y, W,H); } 

		virtual void drawin(graphics::ImageTarget2D& Target, graphics::SizeController2D &controller, int X, int Y, int W, int H) 
		{ drawto(Target, controller, X,Y, W,H); };

		virtual int calculatewidth (int w=-1) const { return getimagewidth(); }
		virtual int calculateheight(int h=-1) const { return getimageheight(); }

		virtual int calculatewidth (const graphics::SizeController2D &controller, int w=-1) const  { return calculatewidthusing(controller,w); }
		virtual int calculateheight(const graphics::SizeController2D &controller, int h=-1) const  { return calculateheightusing(controller,h); }

		virtual void draw(graphics::ImageTarget2D& Target, int X1,int Y1,int X2,int Y2,int X3,int Y3,int X4,int Y4, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) 
		{ drawto(Target, X1,Y1, X2,Y2, X3,Y3, X4,Y4, S1,U1, S2,U2, S3,U3, S4,U4); }
		virtual void draw(graphics::ImageTarget2D& Target, int X1,int Y1,int X2,int Y2,int X3,int Y3,int X4,int Y4) 
		{ drawto(Target, X1,Y1, X2,Y2, X3,Y3, X4,Y4); }
		virtual void drawstretched(graphics::ImageTarget2D& Target, int X, int Y, int W, int H)
		{ drawto(Target, X,Y, W,H); }
		virtual void draw(graphics::ImageTarget2D& Target, graphics::Tiling2D::Type Tiling, int X, int Y, int W, int H)
		{ drawto(Target, Tiling, X,Y, W,H); }
		virtual void draw(graphics::ImageTarget2D& Target, graphics::SizeController2D &controller, int X, int Y, int W, int H)
		{ drawto(Target, controller, X,Y, W,H); }

		virtual int getwidth () const { return getimagewidth(); }
		virtual int getheight() const { return getimageheight(); }

	};


	class AnimationResourceFrame {
	public:
		AnimationResourceFrame(unsigned d=0, unsigned s=0, ImageResource *im=NULL) : Duration(d), Start(s), Image(im) { }

		unsigned Duration;
		unsigned Start;
		ImageResource *Image;
	};


	////
	class AnimationResource : 
		public ResourceBase, public ResizableObjectProvider, 
		public animation::DiscreteInfoProvider 
	{

		friend ResourceBase *LoadAnimationResource(File &File, std::istream &Data, int Size);
		friend class ImageAnimation;
		friend class DiscreteImageAnimation;
	public:
		////03010000h (Gaming, Animation)
		virtual GID::Type getGID() const { return GID::Animation; }
		////Currently does nothing
		virtual bool Save(File &File, std::ostream &Data) { return false; }
		
		////Default constructor
		AnimationResource() : ResourceBase() { FrameCount=TotalLength=0; }

		////Returns the width of the first image
		int GetWidth() const { if(Subitems.getCount()>0) return dynamic_cast<const ImageResource&>(Subitems[0]).GetWidth(); return 0; }
		////Returns the height of the first image
		int GetHeight() const { if(Subitems.getCount()>0) return dynamic_cast<const ImageResource&>(Subitems[0]).GetHeight(); return 0; }
		////Returns number of frames
		int GetFrameCount() const { return FrameCount; }

		virtual ImageAnimation &CreateAnimation(animation::AnimationTimer &controller, bool owner=false) {
			return *new ImageAnimation(*this, controller, owner);
		}

		virtual ImageAnimation &CreateAnimation(bool create=false) {
			return *new ImageAnimation(*this, create);
		}

		virtual ImageResource &ImageAt(unsigned t) { return *Frames[FrameAt(t)].Image; } //Null Image

		//graphics::RectangularGraphic2D &GraphicAt(unsigned t) { return *Frames[FrameAt(t)].Image; } //Null Image

		unsigned GetTotalLength() const { return TotalLength; }

		ImageResource &operator [](int Frame) {
			return dynamic_cast<ImageResource&>(Subitems[Frame]);
		}

		virtual int FrameAt(unsigned t) const;

		virtual int StartOf(unsigned Frame) const;

		virtual int GetDuration(unsigned Frame) const;
		virtual int GetDuration() const;

		virtual int GetNumberofFrames() const;

	protected:
		////Total number of frames that this animation have
		int FrameCount;
		////Frame durations
		std::vector<AnimationResourceFrame> Frames;
		unsigned TotalLength;
	};
} }
