#pragma once

#include "../Engine/GGEMain.h"
#include "../Resource/ResourceBase.h"
#include "../Utils/LinkedList.h"
#include "IWidgetObject.h"
#include "IWidgetContainer.h"

using namespace gre;
using namespace gge;

namespace gge { namespace widgets {
	class VirtualFrame;

	bool keyb_event(input::KeyboardEventType event,int keycode,input::KeyboardModifier::Type modifier,void *data);
	extern utils::LinkedList<VirtualFrame> toplevels;

	class VirtualFrame : public IWidgetContainer {
	protected:

	public:
		VirtualFrame(LayerBase &Parent, int X, int Y, int W, int H, int Order=0) : 
		  IWidgetContainer(Parent, X, Y, W, H, Order) {	
			 keyboardid=input::RegisterKeyboardEvent(this, keyb_event, keyb_event, keyb_event, true);
			 toplevels.Add(this);
		}

		VirtualFrame(LayerBase *Parent, int X, int Y, int W, int H, int Order=0) : 
		  IWidgetContainer(*Parent, X, Y, W, H, Order) {	
			 keyboardid=input::RegisterKeyboardEvent(this, keyb_event, keyb_event, keyb_event, true);
			 toplevels.Add(this);
		}

		VirtualFrame &setOrder(int Order);

		virtual void				Activate();
		virtual void				Deactivate();
		
		virtual void Draw();
		int keyboardid;
		bool activetoplevel;

		virtual ~VirtualFrame() {
			input::UnregisterKeyboardEvent(keyboardid);
			toplevels.Remove(this);
		}
	};
} }
