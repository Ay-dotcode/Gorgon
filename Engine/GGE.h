#pragma once

#include "../Utils/UtilsBase.h"
#include "../Utils/Binary.h"
#include "../Utils/Size2D.h"
#include "../Utils/Rectangle2D.h"
#include "../Utils/Bounds2D.h"
#include "../Utils/Margins.h"


typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned short      WORD;

#pragma warning(disable:4355)


////Gorgon Game Engine
namespace gge {

	namespace Alignment {
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
		inline utils::basic_Point2D<T_> CalculateLocation(Type Align, const utils::basic_Rectangle2D<T_> &target, const utils::basic_Size2D<T_> &object) {
			utils::basic_Point2D<T_> p=target.TopLeft();

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

		template<class T_>
		inline utils::basic_Point2D<T_> CalculateLocation(Type Align, const utils::basic_Bounds2D<T_> &target, const utils::basic_Size2D<T_> &object) {
			utils::basic_Point2D<T_> p=target.TopLeft();

			if(isCenter(Align))
				p.x+=(target.Width()-object.Width)/2;
			else if(isRight(Align))
				p.x+=target.Width()-object.Width;

			if(isMiddle(Align))
				p.y+=(target.Height()-object.Height)/2;
			else if(isRight(Align))
				p.y+=target.Height()-object.Height;

			return p;
		}

		template<class T_>
		inline utils::basic_Point2D<T_> CalculateLocation(Type Align, const utils::basic_Rectangle2D<T_> &target, const utils::basic_Size2D<T_> &object, const utils::Margins &margins) {
			utils::basic_Point2D<T_> p=target.TopLeft()+margins.TopLeft();

			if(isCenter(Align))
				p.x+=(target.Width-object.Width-margins.TotalX())/2;
			else if(isRight(Align))
				p.x+=target.Width-object.Width-margins.TotalX();

			if(isMiddle(Align))
				p.y+=(target.Height-object.Height-margins.TotalY())/2;
			else if(isRight(Align))
				p.y+=target.Height-object.Height-margins.TotalY();

			return p;
		}

		template<class T_>
		inline utils::basic_Point2D<T_> CalculateLocation(Type Align, const utils::basic_Bounds2D<T_> &target, const utils::basic_Size2D<T_> &object, const utils::Margins &margins) {
			utils::basic_Point2D<T_> p=target.TopLeft()+margins.TopLeft();

			if(isCenter(Align))
				p.x+=(target.Width()-object.Width-margins.TotalX())/2;
			else if(isRight(Align))
				p.x+=target.Width()-object.Width-margins.TotalX();

			if(isMiddle(Align))
				p.y+=(target.Height()-object.Height-margins.TotalY())/2;
			else if(isRight(Align))
				p.y+=target.Height()-object.Height-margins.TotalY();

			return p;
		}

		inline bool isLeft(Type t) {
			return (t&Mask_Horizontal) == Left;
		}

		inline bool isRight(Type t) {
			return (t&Mask_Horizontal) == Right;
		}

		inline bool isCenter(Type t) {
			return (t&Mask_Horizontal) == Center;
		}

		inline bool isTop(Type t) {
			return (t&Mask_Vertical) == Top;
		}

		inline bool isBottom(Type t) {
			return (t&Mask_Vertical) == Bottom;
		}

		inline bool isMiddle(Type t) {
			return (t&Mask_Vertical) == Middle;
		}

		inline bool IsValid(Type t) {
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

		inline Type setLeft(Type t) {
			return (Type)( (t&Mask_Vertical) | Left);
		}

		inline Type setRight(Type t) {
			return (Type)( (t&Mask_Vertical) | Right);
		}

		inline Type setCenter(Type t) {
			return (Type)( (t&Mask_Vertical) | Center);
		}

		inline Type setTop(Type t) {
			return (Type)( (t&Mask_Horizontal) | Top);
		}

		inline Type setBottom(Type t) {
			return (Type)( (t&Mask_Horizontal) | Bottom);
		}

		inline Type setMiddle(Type t) {
			return (Type)( (t&Mask_Horizontal) | Middle);
		}
	}; 

}
