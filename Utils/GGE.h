#pragma once

#include "math.h"
#include "memory.h"
#include "Binary.h"


typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;

#ifndef NULL
#define NULL	0
#endif

#include "Point2D.h"
#include "Bounds2D.h"
#include "Rectangle2D.h"

////Gorgon Game Engine
namespace gge {

	template <class _T>
	inline basic_Rectangle2D<_T>::basic_Rectangle2D(const basic_Bounds2D<_T> &bounds) :
		Left(bounds.Left), Top(bounds.Top), Width(bounds.Width()), Height(bounds.Height())
	{ }

	template <class _T>
	inline basic_Rectangle2D<_T>::operator basic_Bounds2D<_T>() {
		return basic_Bounds2D<_T>(*this);
	}

	template <class _T>
	inline basic_Rectangle2D<_T>& basic_Rectangle2D<_T>::operator =(const basic_Bounds2D<_T> &bounds) {
		Left=bounds.Left;
		Top=bounds.Top;
		Width=bounds.Width();
		Height=bounds.Height();

		return *this;
	}

	template <class _T>
	inline basic_Bounds2D<_T>::basic_Bounds2D(const basic_Rectangle2D<_T> &rectangle) :
		Left(rectangle.Left), Top(rectangle.Top), Right(rectangle.Right()), Bottom(rectangle.Bottom())
	{ }

	template <class _T>
	inline basic_Bounds2D<_T>::operator basic_Rectangle2D<_T>() {
		return basic_Rectangle2D<_T>(*this);
	}

	template <class _T>
	inline basic_Bounds2D<_T>& basic_Bounds2D<_T>::operator =(const basic_Rectangle2D<_T> &rect) {
		Left=rect.Left;
		Top=rect.Top;
		Right=rect.Right();
		Bottom=rect.Bottom();

		return *this;
	}


/*
	////This structure defines bounds of a 2D graphical object
	struct Bounds {
		int Left;
		int Top;
		int Right;
		int Bottom;

		Bounds() { }
		////Filling constructor
		Bounds(int l, int t, int r, int b) {
			Left=l;
			Top=t;
			Right=r;
			Bottom=b;
		}
		////Filling constructor
		Bounds(Point pnt, int w, int h) {
			Left=pnt.x;
			Top=pnt.y;
			Right=pnt.x+w;
			Bottom=pnt.y+h;
		}

		Bounds &operator = (Rectangle &r);

		Bounds &operator = (Bounds &r) {
			Left=r.Left;
			Top=r.Top;
			Right=r.Right;
			Bottom=r.Bottom;

			return *this;
		}

		bool operator == (Bounds &r) {
			if(Left==r.Left && Top==r.Top && Right==r.Right && Bottom==r.Bottom)
				return true;

			return false;
		}

		bool operator == (Rectangle &r);

		////Calculates and returns the width of the region
		int Width() { return Right -Left; }
		////Calculates and returns the height of the region
		int Height(){ return Bottom-Top;  }
	};
	////This structure defines properties of a 2D rectangle
	struct Rectangle {
		int Left;
		int Top;
		int Width;
		int Height;

		Rectangle() { }
		////Filling constructor
		Rectangle(int l, int t, int w, int h) {
			Left=l;
			Top=t;
			Width=w;
			Height=h;
		}
		////Filling constructor
		Rectangle(Point pnt, int w, int h) {
			Left=pnt.x;
			Top=pnt.y;
			Width=w;
			Height=h;
		}

		Rectangle &operator = (Rectangle &r) {
			Left=r.Left;
			Top=r.Top;
			Width=r.Width;
			Height=r.Height;

			return *this;
		}

		Rectangle &operator = (Bounds &r) {
			Left=r.Left;
			Top=r.Top;
			Width=r.Width();
			Height=r.Height();

			return *this;
		}

		operator Bounds() {
			Bounds ret;
			ret.Left=Left;
			ret.Top=Top;
			ret.Right=Right();
			ret.Bottom=Bottom();

			return ret;
		}

		bool operator == (Bounds &r) {
			if(Left==r.Left && Top==r.Top && Width==r.Width() && Height==r.Height())
				return true;

			return false;
		}

		bool operator == (Rectangle &r) {
			if(Left==r.Left && Top==r.Top && Width==r.Width && Height==r.Height)
				return true;

			return false;
		}

		////Calculates and returns the width of the region
		int Right() { return Left+Width; }
		////Calculates and returns the height of the region
		int Bottom(){ return Top+Height;  }
	};
	////This structure defines properties of a 2D rectangle
	struct Rectanglef {
		float Left;
		float Top;
		float Width;
		float Height;

		Rectanglef() { }
		////Filling constructor
		Rectanglef(float l, float t, float w, float h) {
			Left=l;
			Top=t;
			Width=w;
			Height=h;
		}
		////Filling constructor
		Rectanglef(Pointf pnt, float w, float h) {
			Left=pnt.x;
			Top=pnt.y;
			Width=w;
			Height=h;
		}

		Rectanglef(gge::Rectangle &rect) {
			Left=(float)rect.Left;
			Top=(float)rect.Top;
			Width=(float)rect.Width;
			Height=(float)rect.Height;
		}

		////Calculates and returns the width of the region
		float Bottom() { return Left+Width; }
		////Calculates and returns the height of the region
		float Right(){ return Top+Height;  }
	};*/

	////2D alignment constants
	enum Alignment {
		ALIGN_LEFT=8,
		ALIGN_RIGHT=16,
		ALIGN_CENTER=32,

		ALIGN_TOP=1,
		ALIGN_BOTTOM=2,
		ALIGN_MIDDLE=4,

		ALIGN_MASK_VERTICAL=B8(00000111),
		ALIGN_MASK_HORIZONTAL=B8(00111000),

		ALIGN_TOP_LEFT=ALIGN_TOP | ALIGN_LEFT,
		ALIGN_TOP_CENTER=ALIGN_TOP | ALIGN_CENTER,
		ALIGN_TOP_RIGHT=ALIGN_TOP | ALIGN_RIGHT,
		ALIGN_MIDDLE_LEFT=ALIGN_MIDDLE | ALIGN_LEFT,
		ALIGN_MIDDLE_CENTER=ALIGN_MIDDLE | ALIGN_CENTER,
		ALIGN_MIDDLE_RIGHT=ALIGN_MIDDLE | ALIGN_RIGHT,
		ALIGN_BOTTOM_LEFT=ALIGN_BOTTOM | ALIGN_LEFT,
		ALIGN_BOTTOM_CENTER=ALIGN_BOTTOM | ALIGN_CENTER,
		ALIGN_BOTTOM_RIGHT=ALIGN_BOTTOM | ALIGN_RIGHT,
	};
}
