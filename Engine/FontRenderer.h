#pragma once

#include "../Engine/Graphics.h"

namespace gre {
	class ResourceFile;
}

namespace gge {

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

	////Base type for shadow parameters
	struct ShadowParams {

		////Types of available text shadows
		enum ShadowTypes {
			None=0,
			Flat=1
		};

		////Type of the shadow
		ShadowTypes Type;
		Point Offset;
		////Shadow color
		RGBint Color;

		ShadowParams() : Type(None), Offset(Point(1,2)), Color(0x60808080) 
		{ }

		ShadowParams(ShadowTypes Type, RGBint color, int XDist, int YDist) : 
			Type(Type), Offset(Point(XDist, YDist)), Color(color) 
		{ }

		ShadowParams(ShadowTypes Type, RGBint color, Point Offset=Point(1,2)) : 
			Type(Type), Color(color), Offset(Offset) 
		{ }

		static ShadowParams Load(gre::ResourceFile* file,FILE* gfile,int sz) {
			ShadowParams s;

			fread(&s.Type, 4,1, gfile);

			if(s.Type==ShadowParams::Flat) {
				fread(&s.Offset, 4,2, gfile);
				fread(&s.Color, 4,1, gfile);
			}

			return s;
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

	class FontRenderer {
		friend class Font;
	protected:
		////Prints the given text to the target using given color.
		virtual void Print(I2DColorizableGraphicsTarget *target, int X, int Y, string Text, RGBint Color, ShadowParams Shadow=ShadowParams())=0;
		////Prints the given text to the target using given color. Text is wrapped and aligned as necessary
		virtual void Print(I2DColorizableGraphicsTarget *target, int X, int Y, int W, string Text, RGBint Color, TextAlignment Align=TEXTALIGN_LEFT, ShadowParams Shadow=ShadowParams())=0;
		////This method is extended to cover meta functionality for advanced text rendering
		virtual void Print(I2DColorizableGraphicsTarget *target, int X, int Y, int W, string Text, RGBint Color, EPrintData *Data, int DataLen, TextAlignment Align=TEXTALIGN_LEFT, ShadowParams Shadow=ShadowParams())=0;
		////This method is extended to cover meta functionality for advanced text rendering. This function does not render the given text
		/// it only processes meta data
		virtual void Print_Test(int X, int Y, int W, string Text, EPrintData *Data, int DataLen, TextAlignment Align)=0;

		virtual int FontHeight()=0;
		virtual int TextWidth(string Text)=0;
	};

}