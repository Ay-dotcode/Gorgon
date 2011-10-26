#pragma once

#include "GRE.h"
#include "ResourceBase.h"
#include "ImageResource.h"
#include "../Engine/FontRenderer.h"
#include <map>

namespace gge { namespace resource {
	class File;



	////This function loads a bitmap font from the given file
	ResourceBase *LoadBitmapFontResource(File &File, std::istream &Data, int Size);

	////This is bitmap font. Bitmap fonts contains every character as images. It has its
	/// pros and cons. Being bitmap, these fonts do not require extra rendering. They are
	/// independent of OS and does not require any additional libraries. However, they have
	/// no scaling, rotation capabilities without losing detail.
	class BitmapFontResource : public ResourceBase, public FontRenderer {
		friend ResourceBase *LoadBitmapFontResource(File &File, std::istream &Data, int Size);
		friend class Font;
	public:
		////Size of the tabs in spaces, default is 4
		int Tabsize;
		////03020000h (Game, Bitmap font)
		virtual GID::Type getGID() const { return GID::Font; }
		////Currently does nothing
		virtual bool Save(File &File, std::ostream &Data) { return false; }

		////Default constructor
		BitmapFontResource() : ResourceBase(), noshadows(false) { 
			Tabsize=4; 
			memset(Characters, 0, 256*sizeof(ImageResource*)); 
			VerticalSpacing=22;

			Shadows[0]=this;
		}

		////Images that represents characters. 8bit ASCII encoding is used. 
		/// An image might be used in more than one character.
		ImageResource *Characters[256];

		////Horizontal separation distance between two characters
		int Seperator;
		////Vertical spacing of this font (px)
		int VerticalSpacing;
		////Baseline from the top
		int Baseline;

		BitmapFontResource &Blur(float amount, int windowsize=-1);


	//protected:
		////Prints the given text to the target using given color.
		virtual void Print(graphics::ColorizableImageTarget2D *target, int X, int Y, const std::string &Text, graphics::RGBint Color, ShadowParams Shadow=ShadowParams());
		////Prints the given text to the target using given color. Text is wrapped and aligned as necessary
		virtual void Print(graphics::ColorizableImageTarget2D *target, int X, int Y, int W, const std::string &Text, graphics::RGBint Color, TextAlignment::Type Align=TextAlignment::Left, ShadowParams Shadow=ShadowParams());
		////This method is extended to cover meta functionality for advanced text rendering
		virtual void Print(graphics::ColorizableImageTarget2D *target, int X, int Y, int W, const std::string &Text, graphics::RGBint Color, EPrintData *Data, int DataLen, TextAlignment::Type Align=TextAlignment::Left, ShadowParams Shadow=ShadowParams());
		////This method is extended to cover meta functionality for advanced text rendering. This function does not render the given text
		/// it only processes meta data
		virtual void Print_Test(int X, int Y, int W, const std::string &Text, EPrintData *Data, int DataLen, TextAlignment::Type Align);
		void Print_Test(utils::Point p, int W, const std::string &Text, EPrintData *Data, int DataLen, TextAlignment::Type Align) {
			Print_Test(p.x, p.y, W, Text, Data, DataLen, Align);
		}



		void Print(graphics::ColorizableImageTarget2D &target, int X, int Y, const std::string &Text, graphics::RGBint Color, ShadowParams Shadow=ShadowParams()) 
		{ Print(&target, X, Y, Text, Color, Shadow); }
		void Print(graphics::ColorizableImageTarget2D &target, int X, int Y, int W, const std::string &Text, graphics::RGBint Color, TextAlignment::Type Align=TextAlignment::Left, ShadowParams Shadow=ShadowParams()) 
		{ Print(&target, X, Y, W, Text, Color, Align, Shadow); }
		void Print(graphics::ColorizableImageTarget2D &target, int X, int Y, int W, const std::string &Text, graphics::RGBint Color, EPrintData *Data, int DataLen, TextAlignment::Type Align=TextAlignment::Left, ShadowParams Shadow=ShadowParams())
		{ Print(&target, X, Y, W, Text, Color, Data, DataLen, Align, Shadow); }

		////Returns the height of this font, all characters have same height
		virtual int FontHeight() { return VerticalSpacing; }
		////Returns the width of the given text
		virtual int TextWidth(const std::string &Text) { 
			int i, w=0; 
			const char *text=Text.c_str(); 
			for(i=0;text[i];i++) 
				w+=Characters[(unsigned char)text[i]]->GetWidth(); 
		
			return w; 
		}
		virtual int TextHeight(const std::string &Text, int W);
		virtual int FontBaseline() { return Baseline; }

		virtual void Prepare(GGEMain &main, File &file);

		virtual ~BitmapFontResource();

		std::map<float, BitmapFontResource*> Shadows;

	protected:
		bool noshadows;
		File *file;
	};
} }