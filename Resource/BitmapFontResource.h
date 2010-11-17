#pragma once

#include "GRE.h"
#include "ResourceBase.h"
#include "ImageResource.h"

namespace gre {
	class ResourceFile;
	
	////Text alignment constants
	enum TextAlignment {
		TEXTALIGN_LEFT=8,
		TEXTALIGN_CENTER=32,
		TEXTALIGN_RIGHT=16
	};

	////Extended print meta data types. This enum can be changed
	/// breaking compatibility
	enum EPrintMetaDataTypes {
		////This is not used
		EMT_None=0,
		////Simple spacing, gives the specified amount of
		/// pixels as space between two characters
		EMT_Spacing=1,
		////Detects the pixel position of a given character
		EMT_PositionDetect,
		////Changes the color after a given point
		EMT_Color,
		////Changes shadow color after a given point
		EMT_ShadowColor,
		////Detects the character that is closest to the given point
		EMT_CharacterDetect,
		////Toggles word wrap
		EMT_Wrap
	};

	////Types of available text shadows
	enum ShadowTypes {
		None=0,
		Flat=1
	};
	////Base type for shadow parameters
	struct ShadowParams {
		////Type of the shadow
		ShadowTypes Type;
		////X Distance of shadow from text
		int XDist;
		////Y Distance of shadow from text
		int YDist;
		////Shadow color
		RGBint Color;

		ShadowParams() : Color(0x60808080) {
			Type=None;
			XDist=1;
			YDist=2;
		}

		ShadowParams(ShadowTypes Type, RGBint color, int XDist=1, int YDist=2) : Color(color) {
			this->Type=Type;
			this->XDist=XDist;
			this->YDist=YDist;
		}
	};
	////Extended print data structure, this structure can be changed
	struct EPrintData {
		////Type of the meta data
		EPrintMetaDataTypes Type;
		////Position of the related character
		int CharPosition;
		////Input data, depending on the type one of the
		/// union element will be used
		union {
			struct {
				int x,y;
			} position;
			unsigned int color;
			int value;
		} In;
		////Input data, depending on the type one of the
		/// union element will be used
		union {
			struct {
				int x,y;
			} position;
			int value;
		} Out;

		////Default constuctor
		EPrintData() { 
			CharPosition=-1;
			Type=EMT_None;
			In.color=0x0;
			Out.position.x=0;
			Out.position.y=0;
		}
	};


	////This function loads a bitmap font from the given file
	ResourceBase *LoadBitmapFontResource(ResourceFile* File, FILE* Data, int Size);

	////This is bitmap font. Bitmap fonts contains every character as images. It has its
	/// pros and cons. Being bitmap, these fonts do not require extra rendering. They are
	/// independent of OS and does not require any additional libraries. However, they have
	/// no scaling, rotation capabilities without losing detail.
	class BitmapFontResource : public ResourceBase {
		friend ResourceBase *LoadBitmapFontResource(ResourceFile* File, FILE* Data, int Size);
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

		////Horizontal seperation distance between two characters
		short Seperator;
		////Vertical spacing of this font, this value is multiplicative
		/// where 1 is 100% of the character height.
		float VerticalSpacing;

		////Prints the given text to the target using given color.
		void Print(I2DColorizableGraphicsTarget *target, int X, int Y, string Text, RGBint Color, ShadowParams Shadow=ShadowParams());
		void Print(I2DColorizableGraphicsTarget &target, int X, int Y, string Text, RGBint Color, ShadowParams Shadow=ShadowParams()) 
		{ Print(&target, X, Y, Text, Color, Shadow); }
		////Prints the given text to the target using given color. Text is wrapped and aligned as necessary
		void Print(I2DColorizableGraphicsTarget *target, int X, int Y, int W, string Text, RGBint Color, TextAlignment Align=TEXTALIGN_LEFT, ShadowParams Shadow=ShadowParams());
		void Print(I2DColorizableGraphicsTarget &target, int X, int Y, int W, string Text, RGBint Color, TextAlignment Align=TEXTALIGN_LEFT, ShadowParams Shadow=ShadowParams()) 
		{ Print(&target, X, Y, W, Text, Color, Align, Shadow); }
		////This method is extended to cover meta functionality for advanced text rendering
		void Print(I2DColorizableGraphicsTarget *target, int X, int Y, int W, string Text, RGBint Color, EPrintData *Data, int DataLen, TextAlignment Align=TEXTALIGN_LEFT, ShadowParams Shadow=ShadowParams());
		void Print(I2DColorizableGraphicsTarget &target, int X, int Y, int W, string Text, RGBint Color, EPrintData *Data, int DataLen, TextAlignment Align=TEXTALIGN_LEFT, ShadowParams Shadow=ShadowParams())
		{ Print(&target, X, Y, W, Text, Color, Data, DataLen, Align, Shadow); }
		////This method is extended to cover meta functionality for advanced text rendering. This function does not render the given text
		/// it only processes meta data
		void Print_Test(int X, int Y, int W, string Text, EPrintData *Data, int DataLen, TextAlignment Align);

		////Returns the height of this font, all characters have same height
		int FontHeight() { return Characters['T']->Height; }
		////Returns the width of the given text
		int TextWidth(string Text) { 
			int i, w=0; 
			const char *text=Text.c_str(); 
			for(i=0;text[i];i++) 
				w+=Characters[(unsigned char)text[i]]->Width; 
		
			return w; 
		}
	};
}