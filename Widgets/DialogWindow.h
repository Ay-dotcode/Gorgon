#pragma once

#include "Window.h"

namespace gge { namespace widgets {

	class DialogWindow : public Window {
	public:
		DialogWindow() : Window() {

			if(WR.Panels.DialogWindow)
				setblueprint(*WR.Panels.DialogWindow);

		}

		void AddDialogButton(WidgetBase &item) {
			dialogbuttons.Add(item);
			placedialogbutton(item);
		}

		void InsertDialogButton(WidgetBase &item, const WidgetBase &before) {
			dialogbuttons.Insert(item, before);
			placedialogbutton(item);
		}

		void RemoveDialogButton(WidgetBase &item) {
			dialogbuttons.Remove(item);
			item.Detach();
			Base::adjustcontrols();
		}

		void DeleteDialogButton(WidgetBase &item) {
			dialogbuttons.Delete(item);
			item.Detach();
			Base::adjustcontrols();
		}

		void ClearDialogButtons() {
			for(auto i=dialogbuttons.First();i.isValid();i.Next())
				i->Detach();

			dialogbuttons.Clear();
			Base::adjustcontrols();
		}

		int GetDialogButtonCount() const {
			return dialogbuttons.getCount();
		}

		virtual void ForcedRollDown() {
			Window::ForcedRollDown();
			dialogcontrols.BaseLayer.isVisible=true;
		}

		virtual void ForcedRollUp() {
			Window::ForcedRollUp();
			dialogcontrols.BaseLayer.isVisible=false;
		}
		void wr_loaded() {
			if(!blueprintmodified && WR.Panels.DialogWindow)
				setblueprint(*WR.Panels.DialogWindow);
		}

	};

}}
