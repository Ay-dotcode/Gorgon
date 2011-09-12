#pragma once

#include "FontRenderer.h"
#include "../Resource/ResourceFile.h"

namespace gge { namespace resource {
	class FontTheme; 
} }

namespace gge {
	class FontInitiator;

	class Font {
	public:
		enum FontStyle {
			Normal	=0,
			Bold	  ,
			Italic	  ,
			Small	  ,
			H1		  ,
			H2		  ,
			H3		  ,
		};



		Font::Font();
		Font(const Font &);
		Font(resource::FontTheme &Theme, graphics::RGBint Color=graphics::RGBint(0xf0001000), FontStyle Style=Normal, ShadowParams Shadow=ShadowParams());
		Font(resource::FontTheme *Theme, graphics::RGBint Color=graphics::RGBint(0xf0001000), FontStyle Style=Normal, ShadowParams Shadow=ShadowParams());

		Font &operator =(const Font &);
		Font &operator =(const FontStyle);


		static FontInitiator Load(resource::File &File, std::istream &Data, int Size);


		resource::FontTheme *Theme;
		FontStyle Style;
		graphics::RGBint Color;
		ShadowParams Shadow;

		//Info functions
		int FontHeight();
		int FontBaseline();
		int TextWidth(const std::string &Text);
		int TextHeight(const std::string &Text, int W);



		//Print Functions
		////Prints the given text to the target using given color.
		void Print(graphics::ColorizableImageTarget2D *target, int X, int Y, const std::string &Text);
		////Prints the given text to the target using given color. Text is wrapped and aligned as necessary
		void Print(graphics::ColorizableImageTarget2D *target, int X, int Y, int W, const std::string &Text, TextAlignment::Type Align=TextAlignment::Left);
		////This method is extended to cover meta functionality for advanced text rendering
		void Print(graphics::ColorizableImageTarget2D *target, int X, int Y, int W, const std::string &Text, EPrintData *Data, int DataLen, TextAlignment::Type Align=TextAlignment::Left);
		////This method is extended to cover meta functionality for advanced text rendering. This function does not render the given text
		/// it only processes meta data
		void Print_Test(int X, int Y, int W, const std::string &Text, EPrintData *Data, int DataLen, TextAlignment::Type Align=TextAlignment::Left);
		void Print_Test(utils::Point p, int W, const std::string &Text, EPrintData *Data, int DataLen, TextAlignment::Type Align=TextAlignment::Left) {
			Print_Test(p.x, p.y, W, Text, Data, DataLen, Align);
		}


		//Target references
		void Print(graphics::ColorizableImageTarget2D &target, int X, int Y, const std::string &Text) 
		{ Print(&target, X, Y, Text); }
		void Print(graphics::ColorizableImageTarget2D &target, int X, int Y, int W, const std::string &Text, TextAlignment::Type Align=TextAlignment::Left) 
		{ Print(&target, X, Y, W, Text, Align); }
		void Print(graphics::ColorizableImageTarget2D &target, int X, int Y, int W, const std::string &Text, EPrintData *Data, int DataLen, TextAlignment::Type Align=TextAlignment::Left)
		{ Print(&target, X, Y, W, Text, Data, DataLen, Align); }

		void Print(graphics::ColorizableImageTarget2D &target, utils::Point p, const std::string &Text) 
		{ Print(&target, p.x, p.y, Text); }
		void Print(graphics::ColorizableImageTarget2D &target, utils::Point p, int W, const std::string &Text, TextAlignment::Type Align=TextAlignment::Left) 
		{ Print(&target, p.x, p.y, W, Text, Align); }
		void Print(graphics::ColorizableImageTarget2D &target, utils::Point p, int W, const std::string &Text, EPrintData *Data, int DataLen, TextAlignment::Type Align=TextAlignment::Left)
		{ Print(&target, p.x, p.y, W, Text, Data, DataLen, Align); }

		void Print(graphics::ColorizableImageTarget2D *target, utils::Point p, const std::string &Text) 
		{ Print(target, p.x, p.y, Text); }
		void Print(graphics::ColorizableImageTarget2D *target, utils::Point p, int W, const std::string &Text, TextAlignment::Type Align=TextAlignment::Left) 
		{ Print(target, p.x, p.y, W, Text, Align); }
		void Print(graphics::ColorizableImageTarget2D *target, utils::Point p, int W, const std::string &Text, EPrintData *Data, int DataLen, TextAlignment::Type Align=TextAlignment::Left)
		{ Print(target, p.x, p.y, W, Text, Data, DataLen, Align); }

		FontRenderer *getRenderer();
	protected:
	};

	class FontInitiator {
	public:
		FontInitiator() : file(NULL) { }
		utils::SGuid guid_theme;
		Font::FontStyle Style;
		graphics::RGBint Color;
		ShadowParams Shadow;
		resource::File *file;

		operator Font();
	};

}