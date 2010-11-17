#pragma once

#include "GGE.h"
#include "stdlib.h"

namespace gge {
	////Returns a random number between Min and Max
	/// Min is inclusive while Max is not
	inline int getRandom(int Min,int Max) {
		return rand()%(Max-Min)+Min;
	}
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
	/// of a region
	inline bool isinrect(int x,int y,gge::Rectangle rect) {
		return
			x>rect.Left	&&
			x<rect.Right() &&
			y>rect.Top	&&
			y<rect.Bottom();
	}

	inline int Round(float num) {
		if(floor(num)==num)
			return (int)num;

		if(num>0 && num-floor(num)>0.5) 
			return (int)num+1; 
		
		if(num<0 && num-floor(num)<0.5) 
			return (int)num-1; 
		
		return (int)num; 
	}
	
	template <class T_, class U_>
	T_ Max(T_ num1, U_ num2) { return num1>num2 ? num1 : num2; }
	
	template <class T_, class U_>
	T_ Min(T_ num1, U_ num2) { return num1<num2 ? num1 : num2; }
}
