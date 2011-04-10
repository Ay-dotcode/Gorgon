#include "VirtualFrame.h"

namespace gge { namespace widgets {
	utils::LinkedList<VirtualFrame> toplevels;

	void DeactivateAll() {
		VirtualFrame *frm;
		utils::LinkedListIterator<VirtualFrame> it=toplevels;
		while(frm=it) {
			frm->Deactivate();
		}
	}

	void VirtualFrame::Draw() {
		if(!isVisible()) return;
		utils::LinkedListOrderedIterator<IWidgetObject> it=Subobjects;
		IWidgetObject *item;

		while(item=it) {
			if(item->isVisible())
				item->Draw();
		}
	}

	VirtualFrame &VirtualFrame::setOrder(int Order) {
		BaseLayer.setOrder(Order);

		return *this;
	}

	void VirtualFrame::Activate() {
		isactive=true;
		input::KeyboardEvents.Enable(keyboardid);
	}

	void VirtualFrame::Deactivate() {
		isactive=false;
		input::KeyboardEvents.Disable(keyboardid);
	}

	bool VirtualFrame::keyboard(input::KeyboardEvent params) {
		return keyboard_event(params.event,params.keycode);
	}

} }
