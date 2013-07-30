#pragma once

#include "GRE.h"
#include "Base.h"
#include "../Engine/Graphics.h"
#include "ResizableObject.h"
#include "../Engine/Animation.h"
#include "../Engine/Image.h"
#include "../Encoding/LZMA.h"

#pragma warning(push)
#pragma warning(disable:4250)

namespace gge { namespace resource {
	class File;
	class Image;

	////This function loads a text resource from the given file
	Image *LoadImageResource(File& File, std::istream &Data, int Size);

	////This is image resource that holds information about a single image. It supports
	/// two color modes (ARGB and AL); lzma and jpg compressions
	class Image : 
		public Base, virtual public ResizableObject, virtual public ResizableObjectProvider, 
		virtual public animation::RectangularGraphic2DSequenceProvider, virtual public graphics::ImageTexture,
		public graphics::ImageData, virtual public animation::RectangularGraphic2DAnimation
	{
		friend Image *LoadImageResource(File &File, std::istream &Data, int Size);
	public:
		enum ImageReadError {
			NoError=0,
			ReadError=1, 
			FileNotFound
		};

		enum Compressor {
			Uncompressed=0,
			LZMA=GID::LZMA,
			PNG=GID::PNG,
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

		Image() : animation::Base(), ImageTexture(), ImageData(), 
		CompressionProps(), Palette(), Compression(PNG), LateLoading(false) 
		{
			isLoaded=LeaveData=false;
			animation::Animations.Remove(this);
		}

		Image(int Width, int Height, graphics::ColorMode::Type Mode=graphics::ColorMode::ARGB) : animation::Base(), 
		ImageTexture(), ImageData(), CompressionProps(), Palette(), Compression(PNG), LateLoading(false) 
		{
			this->Resize(Width, Height, Mode);
			animation::Animations.Remove(this);
		}

		bool PNGExport(std::string filename) { return ExportPNG(filename); }
		bool ExportPNG(std::string filename);
		
		////02020000h (Basic, Image)
		virtual GID::Type GetGID() const { return GID::Image; }
		////Loads image data from the file. This function is required for late
		/// loading.
		bool Load(bool force=false);
		bool LoadData(std::istream &in, int size);
		void LoadProperties(std::istream &in, int size);

		virtual void Prepare(GGEMain &main, File &file);
		void Prepare();

		ImageReadError ImportPNG(std::string filename);
		ImageReadError Import(std::string filename);

		////Returns Bytes/Pixel information
		int GetBPP() { return graphics::getBPP(Mode); }
		graphics::ColorMode::Type GetMode() { return Mode; }
 

		////Destroys used data
		void destroy() { Data.RemoveReference(); if(Palette) delete Palette; }

		////Destroys used data
		virtual ~Image() { 
			utils::CheckAndDelete(Palette);
			utils::CheckAndDelete(CompressionProps);
		}

		/* FOR ANIMATION INTERFACES */
		virtual void DeleteAnimation() { } //if used as animation, it will not be deleted
	//TODO ownership has issues in here
		virtual Image &CreateAnimation(animation::Timer &controller, bool owner=false) { return *this; }
		virtual Image &CreateAnimation(bool create=false) { return *this; }

		virtual Image &CreateResizableObject(animation::Timer &controller, bool owner=false) { return *this; }
		virtual Image &CreateResizableObject(bool create=false) { return *this; }

		virtual graphics::RectangularGraphic2D &GraphicAt(unsigned time) { return *this; }

		virtual graphics::Image2D & ImageAt(int time) { return *this; }

		virtual int GetDuration() const	{ return 1; }
		virtual int GetDuration(unsigned Frame) const { return 1; }
		virtual int GetNumberofFrames() const { return 1; }

		//Caller is responsible to supply a time between 0 and GetDuration()-1, if no frame exists it should return -1
		virtual int		 FrameAt(unsigned Time) const { return 0; }
		//Should always return a time between 0 and GetDuration unless Frame does not exists it should return -1
		virtual int		 StartOf(unsigned Frame) const { return 0; }
		virtual	int		 EndOf(unsigned Frame) const { return 1; }
		/* ... */

		Image &Blur(float amount, int windowsize=-1);
		Image &Shadow(float amount, int windowsize=-1);

		virtual int GetWidth() const {
			if(Texture.ID) {
				return ImageTexture::GetWidth();
			}
			else {
				return ImageData::GetWidth();
			}
		}

		virtual int GetHeight() const {
			if(Texture.ID) {
				return ImageTexture::GetHeight();
			}
			else {
				return ImageData::GetHeight();
			}
		}

		using graphics::ImageTexture::drawin;

		//DO NOT MODIFY THE SIZE OF THE BUFFER MANUALLY
		gge::utils::CastableManagedBuffer<Byte> &getdata() { return ImageData::Data; }

		static encoding::LZMA Lzma;

	protected:
		virtual animation::ProgressResult::Type Progress() { return animation::ProgressResult::None; };

		////Compression properties read from file, used for late loading
		Byte *CompressionProps;
		////Compression mode
		Compressor Compression;
		////Size of the image data within the file, used for late loading
		int DataSize;
		////Location of image data within the file, used for late loading
		int DataLocation;
		bool LateLoading;

		void blurargb(float amount, int windowsize, Image *img);
		void bluralpha(float amount, int windowsize, Image *img);

		void shadowargb(float amount, int windowsize, Image *img);
		void shadowalpha(float amount, int windowsize, Image *img);
	};
} }

#pragma warning(pop)
