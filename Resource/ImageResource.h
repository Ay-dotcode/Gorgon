#pragma once

#include "GRE.h"
#include "ResourceBase.h"
#include "../Engine/Graphics.h"
#include "ResizableObject.h"
#include "../Engine/Animation.h"
#include "../Engine/Image.h"

namespace gge { namespace resource {
	class File;
	
	////This function loads a text resource from the given file
	ResourceBase *LoadImageResource(File& File, std::istream &Data, int Size);

	////This is image resource that holds information about a single image. It supports
	/// two color modes (ARGB and AL); lzma and jpg compressions
	class ImageResource : 
		public ResourceBase, public ResizableObject, public graphics::RectangularGraphic2D, public ResizableObjectProvider, 
		protected graphics::ImageData, protected graphics::TextureImage
	{
		friend ResourceBase *LoadImageResource(File &File, std::istream &Data, int Size);
	public:
		enum PNGReadError {
			NoError=0,
			Signature=1,
			ErrorHandlerProblem=2,
			OutofMemory=4,
			UnimplementedType,
			FileNotFound
		};

		////Not used, if paletted image is found, this holds its palette
		Byte *Palette;
		////Whether image is loaded or not. Image that are marked as late loading
		/// are not loaded in initial load request. Image data can be retrieved by
		/// calling Load function.
		bool isLoaded;
		////Whether to leave the data after this image resource is transformed into
		/// an image object. This flag is used by other systems.
		bool LeaveData;

		ImageResource() : animation::AnimationBase(), TextureImage(), ImageData() {
			isLoaded=LeaveData=false; Palette=NULL; 
		}

		ImageResource(int Width, int Height, graphics::ColorMode::Type Mode=graphics::ColorMode::ARGB) : animation::AnimationBase(), TextureImage(), ImageData() {
			this->Resize(Width, Height, Mode);
		}

		bool PNGExport(string filename);
		
		////02020000h (Basic, Image)
		virtual GID::Type getGID() const { return GID::Image; }
		////Currently does nothing
		virtual bool Save(File &File, std::ostream &Data) { return false; }
		////Loads image data from the file. This function is required for late
		/// loading.
		bool Load();

		virtual void Prepare(GGEMain &main);

		PNGReadError ImportPNG(string filename);

		////Returns Bytes/Pixel information
		int GetBPP() { return graphics::getBPP(Mode); }
		graphics::ColorMode::Type GetMode() { return Mode; }
 

		////Destroys used data
		void destroy() { Data.RemoveReference(); if(Palette) delete Palette; }

		////Destroys used data
		virtual ~ImageResource() { if(Palette) delete Palette; }

		using RectangularGraphic2D::GetHeight;
		using RectangularGraphic2D::GetWidth;

		virtual void DeleteAnimation() { } //if used as animation, it will not be deleted
		virtual ImageResource &CreateAnimation(animation::AnimationTimer &controller, bool owner=false) { return *this; }
		virtual ImageResource &CreateAnimation(bool create=false) { return *this; }

		virtual graphics::RectangularGraphic2D &GraphicAt(unsigned time) { return *this; }

		using TextureImage::GetTexture;


	protected:
		virtual animation::ProgressResult::Type Progress() { return animation::ProgressResult::None; };

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

		virtual int getwidth () const { if(Texture.ID) return getimagewidth(); else return ImageData::GetWidth(); }
		virtual int getheight() const { if(Texture.ID) return getimageheight(); else return ImageData::GetHeight(); }


		////The file that this image resource is related, used for late loading
		File *File;
		////Compression properties read from file, used for late loading
		Byte *CompressionProps;
		////Compression mode, not suitable for saving, used for late loading
		int Compression;
		////Size of the image data within the file, used for late loading
		int DataSize;
		////Location of image data within the file, used for late loading
		int DataLocation;
	};
} }
