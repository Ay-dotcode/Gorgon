#pragma once

#include "../Utils/UtilsBase.h"
#include "../Utils/Binary.h"
#include "../Utils/Size2D.h"
#include "../Utils/Rectangle2D.h"


typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned short      WORD;

#pragma warning(disable:4355)


////Gorgon Game Engine
namespace gge {

	class Alignment {
	public:
		enum Type {
			Left	= B8(00001000),
			Right	= B8(00010000),
			Center	= B8(00100000),

			Top		= B8(00000001),
			Bottom	= B8(00000010),
			Middle	= B8(00000100),

			Top_Left		= Top	 | Left		,
			Top_Center		= Top	 | Center	,
			Top_Right		= Top	 | Right	,

			Bottom_Left		= Bottom | Left		,
			Bottom_Right	= Bottom | Right	,
			Bottom_Center	= Bottom | Center	,

			Middle_Left		= Middle | Left		,
			Middle_Center	= Middle | Center	,
			Middle_Right	= Middle | Right	,
		};

		static const int Mask_Vertical		= B8(00000111);
		static const int Mask_Horizontal	= B8(00111000);
		static const int Mask_Used			= B8(00111111);
		static const int Mask_Invalid		= ~Mask_Used;

		template<class T_>
		static utils::basic_Point2D<T_> CalculateLocation(Type Align, utils::basic_Rectangle2D<T_> object, utils::basic_Size2D<T_> target) {
			utils::basic_Point2D<T_> p=object.TopLeft();

			if(isCenter(Align))
				p.x+=(target.Width-object.Width)/2;
			else if(isRight(Align))
				p.x+=target.Width-object.Width;

			if(isMiddle(Align))
				p.y+=(target.Height-object.Height)/2;
			else if(isRight(Align))
				p.y+=target.Height-object.Height;

			return p;
		}

		static bool isLeft(Type t) {
			return (t&Mask_Horizontal) == Left;
		}

		static bool isRight(Type t) {
			return (t&Mask_Horizontal) == Right;
		}

		static bool isCenter(Type t) {
			return (t&Mask_Horizontal) == Center;
		}

		static bool isTop(Type t) {
			return (t&Mask_Vertical) == Top;
		}

		static bool isBottom(Type t) {
			return (t&Mask_Vertical) == Bottom;
		}

		static bool isMiddle(Type t) {
			return (t&Mask_Vertical) == Middle;
		}

		static bool isValid(Type t) {
			if(t&Mask_Invalid)
				return false;

			int h=t&Mask_Horizontal;
			if( !(h==Left || h==Right || h==Center) )
				return false;

			int v=t&Mask_Vertical;
			if( !(v==Top || v==Bottom || v==Middle) )
				return false;

			return true;
		}
	}; 

}
