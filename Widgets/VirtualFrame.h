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

	extern utils::LinkedList<VirtualFrame> toplevels;

	class VirtualFrame : public IWidgetContainer {
	protected:

	public:
		VirtualFrame(LayerBase &Parent, int X, int Y, int W, int H, int Order=0) : 
		  IWidgetContainer(Parent, X, Y, W, H, Order) {	
			 keyboardid=input::KeyboardEvents.Register(this, &VirtualFrame::keyboard);
			 toplevels.Add(this);
		}

		VirtualFrame(LayerBase *Parent, int X, int Y, int W, int H, int Order=0) : 
		  IWidgetContainer(*Parent, X, Y, W, H, Order) {	
			  keyboardid=input::KeyboardEvents.Register(this, &VirtualFrame::keyboard);
			 toplevels.Add(this);
		}

		VirtualFrame &setOrder(int Order);

		virtual void				Activate();
		virtual void				Deactivate();
		
		virtual void Draw();
		utils::ConsumableEvent<>::Token keyboardid;
		bool activetoplevel;

		virtual ~VirtualFrame() {
			input::KeyboardEvents.Unregister(this, &VirtualFrame::keyboardid);
			toplevels.Remove(this);
		}

	protected:
		bool keyboard(input::KeyboardEvent params);
	};
} }
