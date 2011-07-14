//TO BE MOVED


#pragma once

#include "math.h"
#include "memory.h"
#include "Binary.h"
#include "UtilsBase.h"


typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned short      WORD;


#include "Point2D.h"
#include "Bounds2D.h"
#include "Rectangle2D.h"

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

		const int Mask_Vertical	  = B8(00000111);
		const int Mask_Horizontal = B8(00111000);
		const int Mask_Used = B8(00111111);
		const int Mask_Invalid = ~Mask_Used;

		bool isLeft(Type t) {
			return t&Mask_Horizontal == Left;
		}

		bool isRight(Type t) {
			return t&Mask_Horizontal == Right;
		}

		bool isCenter(Type t) {
			return t&Mask_Horizontal == Center;
		}

		bool isTop(Type t) {
			return t&Mask_Vertical == Top;
		}

		bool isBottom(Type t) {
			return t&Mask_Vertical == Bottom;
		}

		bool isMiddle(Type t) {
			return t&Mask_Vertical == Middle;
		}

		bool isValid(Type t) {
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
