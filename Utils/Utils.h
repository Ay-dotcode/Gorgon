#pragma once

#include "GGE.h"
#include "stdlib.h"

namespace gge {
	////Checks whether a given coordinates within the bounds
	/// of a region
	inline bool isinbounds(int x,int y,Bounds bounds) {
		return
			x>bounds.Left	&&
			x<bounds.Right	&&
			y>bounds.Top	&&
			y<bounds.Bottom;
	}
	////Checks whether a given coordinates within the bounds
	/// of a region, move to rect object
	inline bool isinrect(int x,int y,gge::utils::Rectangle rect) {
		return
			x>rect.Left	&&
			x<rect.Right() &&
			y>rect.Top	&&
			y<rect.Bottom();
	}

	inline int Round(float num) {
		return (int)(num+0.5f); 
	}
	
	template <class T_, class U_>
	T_ Max(T_ num1, U_ num2) { return num1>num2 ? num1 : num2; }
	
	template <class T_, class U_>
	T_ Min(T_ num1, U_ num2) { return num1<num2 ? num1 : num2; }
}
