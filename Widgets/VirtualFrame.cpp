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
		input::EnableKeyboardEvent(keyboardid);
	}

	void VirtualFrame::Deactivate() {
		isactive=false;
		input::DisableKeyboardEvent(keyboardid);
	}

	bool keyb_event(input::KeyboardEventType event,int keycode,input::KeyboardModifier::Type modifier,void *data) {
		if(data) {
			return ((VirtualFrame*)data)->keyboard_event(event,keycode,modifier);
		}

		return false;
	}

} }
