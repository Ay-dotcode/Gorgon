#pragma once

#include "GRE.h"
#include "ResourceBase.h"
#include "ImageResource.h"
#include "../Engine/FontRenderer.h"

namespace gge { namespace resource {
	class ResourceFile;



	////This function loads a bitmap font from the given file
	ResourceBase *LoadBitmapFontResource(ResourceFile* File, FILE* Data, int Size);

	////This is bitmap font. Bitmap fonts contains every character as images. It has its
	/// pros and cons. Being bitmap, these fonts do not require extra rendering. They are
	/// independent of OS and does not require any additional libraries. However, they have
	/// no scaling, rotation capabilities without losing detail.
	class BitmapFontResource : public ResourceBase, public FontRenderer {
		friend ResourceBase *LoadBitmapFontResource(ResourceFile* File, FILE* Data, int Size);
		friend class Font;
	public:
		////Size of the tabs in spaces, default is 4
		int Tabsize;
		////03020000h (Game, Bitmap font)
		virtual int getGID() { return GID_FONT; }
		////Currently does nothing
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }

		////Default constructor
		BitmapFontResource() : ResourceBase() { Tabsize=4; memset(Characters, 0, 256*sizeof(ImageResource*)); VerticalSpacing=1.0f; }

		////Images that represents characters. 8bit ASCII encoding is used. 
		/// An image might be used in more than one character.
		ImageResource *Characters[256];

		////Horizontal separation distance between two characters
		short Seperator;
		////Vertical spacing of this font, this value is multiplicative
		/// where 1 is 100% of the character height.
		float VerticalSpacing;

	//protected:
		////Prints the given text to the target using given color.
		virtual void Print(graphics::I2DColorizableGraphicsTarget *target, int X, int Y, string Text, graphics::RGBint Color, ShadowParams Shadow=ShadowParams());
		////Prints the given text to the target using given color. Text is wrapped and aligned as necessary
		virtual void Print(graphics::I2DColorizableGraphicsTarget *target, int X, int Y, int W, string Text, graphics::RGBint Color, TextAlignment Align=TEXTALIGN_LEFT, ShadowParams Shadow=ShadowParams());
		////This method is extended to cover meta functionality for advanced text rendering
		virtual void Print(graphics::I2DColorizableGraphicsTarget *target, int X, int Y, int W, string Text, graphics::RGBint Color, EPrintData *Data, int DataLen, TextAlignment Align=TEXTALIGN_LEFT, ShadowParams Shadow=ShadowParams());
		////This method is extended to cover meta functionality for advanced text rendering. This function does not render the given text
		/// it only processes meta data
		virtual void Print_Test(int X, int Y, int W, string Text, EPrintData *Data, int DataLen, TextAlignment Align);



		void Print(graphics::I2DColorizableGraphicsTarget &target, int X, int Y, string Text, graphics::RGBint Color, ShadowParams Shadow=ShadowParams()) 
		{ Print(&target, X, Y, Text, Color, Shadow); }
		void Print(graphics::I2DColorizableGraphicsTarget &target, int X, int Y, int W, string Text, graphics::RGBint Color, TextAlignment Align=TEXTALIGN_LEFT, ShadowParams Shadow=ShadowParams()) 
		{ Print(&target, X, Y, W, Text, Color, Align, Shadow); }
		void Print(graphics::I2DColorizableGraphicsTarget &target, int X, int Y, int W, string Text, graphics::RGBint Color, EPrintData *Data, int DataLen, TextAlignment Align=TEXTALIGN_LEFT, ShadowParams Shadow=ShadowParams())
		{ Print(&target, X, Y, W, Text, Color, Data, DataLen, Align, Shadow); }

		////Returns the height of this font, all characters have same height
		virtual int FontHeight() { return Characters['T']->getHeight(); }
		////Returns the width of the given text
		virtual int TextWidth(string Text) { 
			int i, w=0; 
			const char *text=Text.c_str(); 
			for(i=0;text[i];i++) 
				w+=Characters[(unsigned char)text[i]]->getWidth(); 
		
			return w; 
		}
	};
} }