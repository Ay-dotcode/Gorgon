#pragma once


#include "../../Engine/Pointer.h"
#include "../../Resource/Base.h"


namespace gge { namespace widgets {

	class WidgetBase;

	class Blueprint {
	public:

		//format: 0xtf, f: from, t: to
		enum FocusType {
			//Not valid unless determining transitions
			Focus_None			= 0x00,
			NotFocused			= 0x01,
			Focused				= 0x02
		};

		struct FocusMode {
			FocusMode() : from(NotFocused), to(Focus_None)
			{ }

			FocusMode(FocusType from, FocusType to) : from(from), to(to)
			{ }

			FocusMode(int i) : from(FocusType(i%0x10)), to(FocusType(i/0x10))
			{ }

			FocusType from : 4;
			FocusType to   : 4;

			bool operator <(const FocusMode &f) const {
				if(to<f.to)
					return true;
				else if(to>f.to)
					return false;
				else						
					return from<f.from;
			}

			bool operator ==(const FocusMode &f) const {
				return from==f.from && to==f.to;
			}

			FocusMode swap() {
				return FocusMode(to,from);
			}
		};


		enum StyleType {
			Style_None	=0,
			Normal		=1,
			Hover		=2,
			Down		=4,
			Disabled	=5,
			Active=Hover,
			Moving=Down,
			Focused_Style=Down
		};

		struct StyleMode {
			StyleMode(StyleType from=Normal, StyleType to=Style_None) : from(from), to(to)
			{ }

			StyleType from;
			StyleType to;

			StyleMode swap() {
				return StyleMode(to, from);
			}
		};

		enum AnimationDirection {
			Missing  = 0,
			Forward  = 1,
			Backward =-1,
		};

		struct AnimationInfo {
			AnimationInfo(AnimationDirection direction=Missing,int duration=-1) : direction(direction), duration(duration)
			{ }

			operator bool() {
				return direction!=Missing;
			}

			AnimationDirection direction;
			int duration;
		};


		Blueprint() : AlphaAnimation(0), Pointer(gge::Pointer::None), DefaultSize(0,0)
		{ }

		int AlphaAnimation;

		utils::Size  DefaultSize;

		gge::Pointer::PointerType Pointer;
	};

}}
