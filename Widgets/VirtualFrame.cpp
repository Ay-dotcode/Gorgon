#include "VirtualFrame.h"

namespace gorgonwidgets {
	LinkedList<VirtualFrame> toplevels;

	void DeactivateAll() {
		VirtualFrame *frm;
		LinkedListIterator<VirtualFrame> it=toplevels;
		while(frm=it) {
			frm->Deactivate();
		}
	}

	void VirtualFrame::Draw() {
		if(!isVisible()) return;
		LinkedListOrderedIterator<IWidgetObject> it=Subobjects;
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
		EnableKeyboardEvent(keyboardid);
	}

	void VirtualFrame::Deactivate() {
		isactive=false;
		DisableKeyboardEvent(keyboardid);
	}

	bool keyb_event(KeyboardEventType event,int keycode,KeyboardModifier modifier,void *data) {
		if(data) {
			return ((VirtualFrame*)data)->keyboard_event(event,keycode,modifier);
		}

		return false;
	}

}