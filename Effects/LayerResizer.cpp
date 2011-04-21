#include "LayerResizer.h"
namespace gge { namespace effects {
	void LayerResizer::Setup(gge::Rectangle From, gge::Rectangle To, int Time) {

		from=Rectangle2D(From);
		current=from;
		to=To;

		Target->X=(int)from.Left;
		Target->Y=(int)from.Top;
		Target->W=(int)from.Width;
		Target->H=(int)from.Height;
		this->progressed=0;

		if(Time) {
			speed.Left=(float)(to.Left-from.Left)/Time;
			speed.Top=(float)(to.Top-from.Top)/Time;
			speed.Width=(float)(to.Width-from.Width)/Time;
			speed.Height=(float)(to.Height-from.Height)/Time;

			this->Play();
		} else {
			speed.Left=0;
			speed.Top=0;
			speed.Width=0;
			speed.Height=0;
			current=from=to;
			Target->X=(int)to.Left;
			Target->Y=(int)to.Top;
			Target->W=(int)to.Width;
			Target->H=(int)to.Height;
		}
	}

	bool LayerResizer::isFinished() {
		return current.Left==to.Left && current.Top==to.Top && current.Width==to.Width && current.Height==to.Height;
	}

	void LayerResizer::Process(int Time) {
		if(from.Left>to.Left) {
			current.Left=from.Left+Time*speed.Left;

			if(current.Left<to.Left)
				current.Left=to.Left;
		} else {
			current.Left=from.Left+Time*speed.Left;
			if(current.Left>to.Left)
				current.Left=to.Left;
		}
		Target->X=Round(current.Left);


		if(from.Top>to.Top) {
			current.Top=from.Top+Time*speed.Top;

			if(current.Top<to.Top)
				current.Top=to.Top;
		} else {
			current.Top=from.Top+Time*speed.Top;
			if(current.Top>to.Top)
				current.Top=to.Top;
		}
		Target->Y=Round(current.Top);


		if(from.Width>to.Width) {
			current.Width=from.Width+Time*speed.Width;

			if(current.Width<to.Width)
				current.Width=to.Width;
		} else {
			current.Width=from.Width+Time*speed.Width;
			if(current.Width>to.Width)
				current.Width=to.Width;
		}
		Target->W=Round(current.Width);


		if(from.Height>to.Height) {
			current.Height=from.Height+Time*speed.Height;

			if(current.Height<to.Height)
				current.Height=to.Height;
		} else {
			current.Height=from.Height+Time*speed.Height;
			if(current.Height>to.Height)
				current.Height=to.Height;
		}
		Target->H=Round(current.Height);
	}
} }
