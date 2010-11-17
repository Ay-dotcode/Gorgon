#pragma once

#include "GRE.h"
#include "ResourceBase.h"
#include "../Engine/Graphics.h"

namespace gre {
	class ResourceFile;
	
	////This function loads a text resource from the given file
	ResourceBase *LoadImageResource(ResourceFile* File, FILE* Data, int Size);

	enum PNGReadError {
		NoError=0,
		Signature=1,
		ErrorHandlerProblem=2,
		OutofMemory=4,
		UnimplementedType
	};

	////This is image resource that holds information about a single image. It supports
	/// two color modes (ARGB and AL); lzma and jpg compressions
	class ImageResource : public ResourceBase, public Colorizable2DGraphic, public Raw2DGraphic {
		friend ResourceBase *LoadImageResource(ResourceFile* File, FILE* Data, int Size);
	public:
		////Not used, if paletted image is found, this holds its palette
		BYTE *Palette;
		////Whether image is loaded or not. Image that are marked as late loading
		/// are not loaded in initial load request. Image data can be retrived by
		/// calling Load function.
		bool isLoaded;
		////Whether to leave the data after this image resource is transformed into
		/// an image object. This flag is used by other systems.
		bool LeaveData;

		ImageResource() { Width=Height=0; isLoaded=LeaveData=false; Palette=NULL; Mode=ARGB_32BPP; }

		ImageResource(int Width, int Height, ColorMode Mode=ARGB_32BPP) {
			isLoaded=LeaveData=true;
			Palette=NULL;
			this->Resize(Width, Height, Mode); 
		}

		void PNGExport(string filename);
		
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

		////Destroys used data
		void destroy() { Data--; if(Palette) delete Palette; }

		////Destroys used data
		virtual ~ImageResource() { if(Palette) delete Palette; ResourceBase::~ResourceBase(); }

	protected:
		////The file that this image resource is related, used for late loading
		ResourceFile *File;
		////Compression properties read from file, used for late loading
		BYTE *CompressionProps;
		////Compression mode, not suitable for saving, used for late loading
		int Compression;
		////Size of the image data within the file, used for late loading
		int DataSize;
		////Location of image data within the file, used for late loading
		long DataLocation;
	};
}