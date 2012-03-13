#include "LayerResizer.h"
#include "..\Utils\BasicMath.h"

using namespace gge::utils;

namespace gge { namespace effects {
	void LayerResizer::Setup(utils::Rectangle From, utils::Rectangle To, int Time) {

		from=Rectangle2D(From);
		current=from;
		to=To;

		Target->BoundingBox=from;

		if(Controller)
			Controller->ResetProgress();

		if(Time) {
			speed.Left=(float)(to.Left-from.Left)/Time;
			speed.Top=(float)(to.Top-from.Top)/Time;
			speed.Width=(float)(to.Width-from.Width)/Time;
			speed.Height=(float)(to.Height-from.Height)/Time;
		} else {
			speed.Left=0;
			speed.Top=0;
			speed.Width=0;
			speed.Height=0;
			current=from=to;
			Target->BoundingBox=to;
		}
	}

	void LayerResizer::Setup( utils::Rectangle To, int Time ) {
		Setup(utils::Rectangle((int)Round(current.Left), (int)Round(current.Top), (int)Round(current.Width), (int)Round(current.Height)), To, Time);
	}

	animation::ProgressResult::Type LayerResizer::Progress() {
		if(current==to)
			return animation::ProgressResult::Finished;

		if(!Controller)
			return animation::ProgressResult::None;

		if(Controller->GetProgress()<0) {
			throw std::runtime_error("LayerMover cannot handle negative animation time.");
		}

		int Time=Controller->GetProgress();

		if(from.Left>to.Left) {
			current.Left=from.Left+Time*speed.Left;

			if(current.Left<to.Left)
				current.Left=to.Left;
		} else {
			current.Left=from.Left+Time*speed.Left;
			if(current.Left>to.Left)
				current.Left=to.Left;
		}
		Target->BoundingBox.Left=(int)Round(current.Left);


		if(from.Top>to.Top) {
			current.Top=from.Top+Time*speed.Top;

			if(current.Top<to.Top)
				current.Top=to.Top;
		} else {
			current.Top=from.Top+Time*speed.Top;
			if(current.Top>to.Top)
				current.Top=to.Top;
		}
		Target->BoundingBox.Top=(int)Round(current.Top);


		if(from.Width>to.Width) {
			current.Width=from.Width+Time*speed.Width;

			if(current.Width<to.Width)
				current.Width=to.Width;
		} else {
			current.Width=from.Width+Time*speed.Width;
			if(current.Width>to.Width)
				current.Width=to.Width;
		}


		if(from.Height>to.Height) {
			current.Height=from.Height+Time*speed.Height;

			if(current.Height<to.Height)
				current.Height=to.Height;
		} else {
			current.Height=from.Height+Time*speed.Height;
			if(current.Height>to.Height)
				current.Height=to.Height;
		}

		Target->BoundingBox.SetSize((int)Round(current.Width), (int)Round(current.Height));


		if(from==to)
			return animation::ProgressResult::Finished;
		else
			return animation::ProgressResult::None;
	}
} }
