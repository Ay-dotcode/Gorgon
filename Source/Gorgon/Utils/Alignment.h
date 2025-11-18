#pragma once

#include "UtilsBase.h"
#include "Binary.h"
#include "Size2D.h"
#include "Rectangle2D.h"
#include "Bounds2D.h"
#include "Margins.h"

namespace Gorgon {

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
		
		inline bool IsLeft(Type t) {
			return (t&Mask_Horizontal) == Left;
		}

		inline bool IsRight(Type t) {
			return (t&Mask_Horizontal) == Right;
		}

		inline bool IsCenter(Type t) {
			return (t&Mask_Horizontal) == Center;
		}

		inline bool IsTop(Type t) {
			return (t&Mask_Vertical) == Top;
		}

		inline bool IsBottom(Type t) {
			return (t&Mask_Vertical) == Bottom;
		}

		inline bool IsMiddle(Type t) {
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

		inline Type SetLeft(Type t) {
			return (Type)( (t&Mask_Vertical) | Left);
		}

		inline Type SetRight(Type t) {
			return (Type)( (t&Mask_Vertical) | Right);
		}

		inline Type SetCenter(Type t) {
			return (Type)( (t&Mask_Vertical) | Center);
		}

		inline Type SetTop(Type t) {
			return (Type)( (t&Mask_Horizontal) | Top);
		}

		inline Type SetBottom(Type t) {
			return (Type)( (t&Mask_Horizontal) | Bottom);
		}

		inline Type SetMiddle(Type t) {
			return (Type)( (t&Mask_Horizontal) | Middle);
		}
		
		template<class T_>
		inline basic_Point2D<T_> CalculateLocation(Type Align, const basic_Rectangle2D<T_> &target, const basic_Size2D<T_> &object) {
			basic_Point2D<T_> p=target.TopLeft();

			if(IsCenter(Align))
				p.x+=(target.Width-object.Width)/2;
			else if(IsRight(Align))
				p.x+=target.Width-object.Width;

			if(IsMiddle(Align))
				p.y+=(target.Height-object.Height)/2;
			else if(IsRight(Align))
				p.y+=target.Height-object.Height;

			return p;
		}

		template<class T_>
		inline basic_Point2D<T_> CalculateLocation(Type Align, const basic_Bounds2D<T_> &target, const basic_Size2D<T_> &object) {
			basic_Point2D<T_> p=target.TopLeft();

			if(IsCenter(Align))
				p.x+=(target.Width()-object.Width)/2;
			else if(IsRight(Align))
				p.x+=target.Width()-object.Width;

			if(IsMiddle(Align))
				p.y+=(target.Height()-object.Height)/2;
			else if(IsRight(Align))
				p.y+=target.Height()-object.Height;

			return p;
		}

		template<class T_>
		inline basic_Point2D<T_> CalculateLocation(Type Align, const basic_Rectangle2D<T_> &target, const basic_Size2D<T_> &object, const Margins &margins) {
			basic_Point2D<T_> p=target.TopLeft()+margins.TopLeft();

			if(IsCenter(Align))
				p.x+=(target.Width-object.Width-margins.TotalX())/2;
			else if(IsRight(Align))
				p.x+=target.Width-object.Width-margins.TotalX();

			if(IsMiddle(Align))
				p.y+=(target.Height-object.Height-margins.TotalY())/2;
			else if(IsRight(Align))
				p.y+=target.Height-object.Height-margins.TotalY();

			return p;
		}

		template<class T_>
		inline basic_Point2D<T_> CalculateLocation(Type Align, const basic_Bounds2D<T_> &target, const basic_Size2D<T_> &object, const Margins &margins) {
			basic_Point2D<T_> p=target.TopLeft()+margins.TopLeft();

			if(IsCenter(Align))
				p.x+=(target.Width()-object.Width-margins.TotalX())/2;
			else if(IsRight(Align))
				p.x+=target.Width()-object.Width-margins.TotalX();

			if(IsMiddle(Align))
				p.y+=(target.Height()-object.Height-margins.TotalY())/2;
			else if(IsRight(Align))
				p.y+=target.Height()-object.Height-margins.TotalY();

			return p;
		}
	}; 

}
