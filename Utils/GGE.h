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

	template <class T_>
	inline basic_Rectangle2D<T_>::basic_Rectangle2D(const basic_Bounds2D<T_> &bounds) :
		Left(bounds.Left), Top(bounds.Top), Width(bounds.Width()), Height(bounds.Height())
	{ }

	template <class T_>
	inline basic_Rectangle2D<T_>::operator basic_Bounds2D<T_>() {
		return basic_Bounds2D<T_>(*this);
	}

	template <class T_>
	inline basic_Rectangle2D<T_>& basic_Rectangle2D<T_>::operator =(const basic_Bounds2D<T_> &bounds) {
		Left=bounds.Left;
		Top=bounds.Top;
		Width=bounds.Width();
		Height=bounds.Height();

		return *this;
	}

	template <class T_>
	inline basic_Bounds2D<T_>::basic_Bounds2D(const basic_Rectangle2D<T_> &rectangle) :
		Left(rectangle.Left), Top(rectangle.Top), Right(rectangle.Right()), Bottom(rectangle.Bottom())
	{ }

	template <class T_>
	inline basic_Bounds2D<T_>::operator basic_Rectangle2D<T_>() {
		return basic_Rectangle2D<T_>(*this);
	}

	template <class T_>
	inline basic_Bounds2D<T_>& basic_Bounds2D<T_>::operator =(const basic_Rectangle2D<T_> &rect) {
		Left=rect.Left;
		Top=rect.Top;
		Right=rect.Right();
		Bottom=rect.Bottom();

		return *this;
	}


	////2D alignment constants
	enum Alignment {
		ALIGN_LEFT=8,
		ALIGN_RIGHT=16,
		ALIGN_CENTER=32,

		ALIGN_TOP=1,
		ALIGN_BOTTOM=2,
		ALIGN_MIDDLE=4,

		ALIGN_MASK_VERTICAL  = B8(00000111),
		ALIGN_MASK_HORIZONTAL= B8(00111000),

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
