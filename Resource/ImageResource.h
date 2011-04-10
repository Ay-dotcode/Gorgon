#pragma once

#include "GRE.h"
#include "ResourceBase.h"
#include "../Engine/Graphics.h"
#include "ResizableObject.h"

namespace gre {
	class ResourceFile;
	
	////This function loads a text resource from the given file
	ResourceBase *LoadImageResource(ResourceFile* File, FILE* Data, int Size);

	enum PNGReadError {
		NoError=0,
		Signature=1,
		ErrorHandlerProblem=2,
		OutofMemory=4,
		UnimplementedType,
		FileNotFound
	};

	////This is image resource that holds information about a single image. It supports
	/// two color modes (ARGB and AL); lzma and jpg compressions
	class ImageResource : public ResourceBase, public Colorizable2DGraphic, public Raw2DGraphic, public ResizableObject {
		friend ResourceBase *LoadImageResource(ResourceFile* File, FILE* Data, int Size);
	public:
		////Not used, if paletted image is found, this holds its palette
		Byte *Palette;
		////Whether image is loaded or not. Image that are marked as late loading
		/// are not loaded in initial load request. Image data can be retrived by
		/// calling Load function.
		bool isLoaded;
		////Whether to leave the data after this image resource is transformed into
		/// an image object. This flag is used by other systems.
		bool LeaveData;

		ImageResource() { 
			Raw2DGraphic::Width=Raw2DGraphic::Height=0; 
			isLoaded=LeaveData=false; Palette=NULL; Mode=ARGB_32BPP; 
			SetResizingOptions(ResizableObject::Single,ResizableObject::Single);
		}

		ImageResource(int Width, int Height, ColorMode Mode=ARGB_32BPP) {
			isLoaded=LeaveData=true;
			Palette=NULL;
			this->Resize(Width, Height, Mode);
			SetResizingOptions(ResizableObject::Single,ResizableObject::Single);
		}

		ImageResource(ImageResource &image, ResizableObject::Tiling Horizontal, ResizableObject::Tiling Vertical) {
			*this=image;
			SetResizingOptions(Horizontal, Vertical);
		}

		ImageResource(ImageResource *image, ResizableObject::Tiling Horizontal, ResizableObject::Tiling Vertical) {
			*this=*image;
			SetResizingOptions(Horizontal, Vertical);
		}

		bool PNGExport(string filename);
		
		////02020000h (Basic, Image)
		virtual int getGID() { return GID_IMAGE; }
		////Currently does nothing
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }
		////Loads image data from the file. This function is required for late
		/// loading.
		bool Load();

		virtual void Prepare(GGEMain *main);

		PNGReadError ImportPNG(string filename);

		////Returns Bytes/Pixel information
		int getBPP() { return gge::getBPP(Mode); }
 
		////Returns the width of the first image
		int getWidth() { return Raw2DGraphic::Width; }
		////Returns the height of the first image
		int getHeight() { return Raw2DGraphic::Height; }


		////Destroys used data
		void destroy() { Data--; if(Palette) delete Palette; }

		////Destroys used data
		virtual ~ImageResource() { if(Palette) delete Palette; }



		//For resizable interface
		ResizableObject::Tiling HorizontalTiling;
		ResizableObject::Tiling VerticalTiling;


		void SetResizingOptions( ResizableObject::Tiling Horizontal, ResizableObject::Tiling Vertical ) {
			this->HorizontalTiling=Horizontal;
			this->VerticalTiling=Vertical;
		}

		virtual void DrawResized(I2DGraphicsTarget *Target, int X, int Y, int W, int H, Alignment Align=ALIGN_MIDDLE_CENTER);
		virtual void DrawResized(I2DGraphicsTarget &Target, int X, int Y, int W, int H, Alignment Align=ALIGN_MIDDLE_CENTER) {
			DrawResized(&Target, X,Y,W,H,Align);
		}
		virtual int  Width(int W=-1) { 
			if(W==-1) return getWidth();

			return HorizontalTiling.Calculate(getWidth(), W); 
		}
		virtual int  Height(int H=-1) { 
			if(H==-1) return getHeight(); 
		
			return VerticalTiling.Calculate(getHeight(), H); 
		}
		virtual void Reset(bool Reverse=false) {}
		virtual void Reverse() {}
		virtual void Play() {}
		virtual void Pause() {}
		virtual void setLoop(bool Loop) {}
		virtual int getDuration() { return 0; }


	protected:
		////The file that this image resource is related, used for late loading
		ResourceFile *File;
		////Compression properties read from file, used for late loading
		Byte *CompressionProps;
		////Compression mode, not suitable for saving, used for late loading
		int Compression;
		////Size of the image data within the file, used for late loading
		int DataSize;
		////Location of image data within the file, used for late loading
		long DataLocation;
	};
}