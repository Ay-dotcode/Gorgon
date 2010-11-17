#pragma once

#include "../Utils/GGE.h"

namespace gge {

	class ResizableObject {
	public:
		virtual void DrawResized(I2DGraphicsTarget *Target, int X, int Y, int W, int H, Alignment Align)=0;
		virtual void DrawResized(I2DGraphicsTarget &Target, int X, int Y, int W, int H, Alignment Align) { DrawResized(&Target, X, Y, W, H, Align); }
		virtual int  Width(int W=-1)=0;
		virtual int  Height(int H=-1)=0;
		virtual void Reset(bool Reverse=false)=0;
		virtual void Reverse()=0;
		virtual void Play()=0;
		virtual void Pause()=0;
		virtual void setLoop(bool Loop)=0;
		virtual int getDuration()=0;
	};
}
