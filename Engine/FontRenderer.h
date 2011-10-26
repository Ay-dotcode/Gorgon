#pragma once

#include "../Engine/Graphics.h"
#include "../Resource/ResourceFile.h"
#include "GraphicTargets2D.h"

namespace gge { namespace resource {
	class File;
} }

namespace gge {

	////Text alignment constants
	namespace TextAlignment {
		enum Type {
			Left		= 8 ,
			Center		= 32,
			Right		= 16,
		};

		inline Type GetHorizontal(Alignment::Type align)  {
			if(Alignment::isLeft(align))
				return Left;
			else if(Alignment::isRight(align))
				return Right;
			else
				return Center;
		}
	};

	////Base type for shadow parameters
	struct ShadowParams {

		////Types of available text shadows
		enum ShadowTypes {
			None=0,
			Flat=1,
			DropShadow=2
		};

		////Type of the shadow
		ShadowTypes Type;
		utils::Point Offset;
		////Shadow color
		graphics::RGBint Color;
		float Blur;

		ShadowParams() : Type(None), Offset(utils::Point(1,2)), Color(0x60808080), Blur(1) 
		{ }

		ShadowParams(ShadowTypes Type, graphics::RGBint color, int XDist, int YDist) : 
		Type(Type), Offset(utils::Point(XDist, YDist)), Color(color) 
		{ }

		ShadowParams(ShadowTypes Type, graphics::RGBint color, utils::Point Offset=utils::Point(1,2)) : 
		Type(Type), Color(color), Offset(Offset) 
		{ }

		ShadowParams(ShadowTypes Type, graphics::RGBint color, float blur, int XDist, int YDist) : 
		Type(Type), Offset(utils::Point(XDist, YDist)), Color(color), Blur(blur)
		{ }

		ShadowParams(ShadowTypes Type, graphics::RGBint color, float blur, utils::Point Offset=utils::Point(1,2)) : 
		Type(Type), Color(color), Offset(Offset), Blur(blur)
		{ }

		static ShadowParams Load(resource::File &File, std::istream &Data, int Size) {
			ShadowParams s;

			s.Type=(ShadowTypes)resource::ReadFrom<int>(Data);

			if(s.Type==ShadowParams::Flat) {
				resource::ReadFrom(Data, s.Offset);
				resource::ReadFrom(Data, s.Color);
			}
			else if(s.Type==ShadowParams::DropShadow) {
				resource::ReadFrom(Data, s.Offset);
				resource::ReadFrom(Data, s.Color);
				resource::ReadFrom(Data, s.Blur);
			}

			return s;
		}
	};

	////Extended print data structure, this structure can be changed
	struct EPrintData {
		////Type of the meta data
		enum EPrintDataTypes {
			////This is not used
			None=0,
			////Simple spacing, gives the specified amount of
			/// pixels as space between two characters
			Spacing=1,
			////Detects the pixel position of a given character
			PositionDetect,
			////Changes the color after a given point
			Color,
			////Changes shadow color after a given point
			ShadowColor,
			////Detects the character that is closest to the given point
			CharacterDetect,
			////Toggles word wrap
			Wrap
		} Type;

		////Position of the related character
		int CharPosition;
		////Input data, depending on the type one of the
		/// union element will be used
		union {
			struct {
				int x,y;

				operator utils::Point() const {
					return utils::Point(x,y);
				}

				void operator =(utils::Point v) {
					x=v.x;
					y=v.y;
				}
			} position;
			unsigned int color;
			int value;
		} In;
		////Input data, depending on the type one of the
		/// union element will be used
		union {
			struct {
				int x,y;

				void operator =(utils::Point v) {
					x=v.x;
					y=v.y;
				}

				operator utils::Point() const {
					return utils::Point(x,y);
				}
			} position;
			int value;
		} Out;

		////Default constructor
		EPrintData() { 
			CharPosition=-1;
			Type=None;
			In.color=0x0;
			Out.position.x=0;
			Out.position.y=0;
		}
	};

	class FontRenderer {
		friend class Font;
	public:
		////Prints the given text to the target using given color.
		virtual void Print(graphics::ColorizableImageTarget2D *target, int X, int Y, const std::string &Text, graphics::RGBint Color, ShadowParams Shadow=ShadowParams())=0;
		////Prints the given text to the target using given color. Text is wrapped and aligned as necessary
		virtual void Print(graphics::ColorizableImageTarget2D *target, int X, int Y, int W, const std::string &Text, graphics::RGBint Color, TextAlignment::Type Align=TextAlignment::Left, ShadowParams Shadow=ShadowParams())=0;
		////This method is extended to cover meta functionality for advanced text rendering
		virtual void Print(graphics::ColorizableImageTarget2D *target, int X, int Y, int W, const std::string &Text, graphics::RGBint Color, EPrintData *Data, int DataLen, TextAlignment::Type Align=TextAlignment::Left, ShadowParams Shadow=ShadowParams())=0;
		////This method is extended to cover meta functionality for advanced text rendering. This function does not render the given text
		/// it only processes meta data
		virtual void Print_Test(int X, int Y, int W, const std::string &Text, EPrintData *Data, int DataLen, TextAlignment::Type Align)=0;

		virtual int FontHeight()=0;
		virtual int FontBaseline()=0;
		virtual int TextWidth(const std::string &Text)=0;
		virtual int TextHeight(const std::string &Text, int W)=0;

	};

}