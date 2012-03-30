#pragma once

#include "Panel/PanelBase.h"

namespace gge { namespace widgets {

	class Panel : public panel::Base {
	public:

		enum DockDirection {
			Left,
			Top,
			Right,
			Bottom
		};

		Panel() : INIT_PROPERTY(Panel, Padding)
		{
			setallowmove(false);
			setupvscroll(true, true, true);

			if(WR.Panel)
				setblueprint(*WR.Panel);
		}

		void FillContainer() {
			if(Container) {
				Move(0,0);
				Resize(Container->GetUsableWidth()-1, Container->GetUsableHeight()-1);
			}
		}

		void Dock(DockDirection direction) {
			if(!Container) return;
			prepare();

			utils::Margins margins(0);
			
			if(getouterborder())
				margins=getouterborder()->BorderWidth;

			switch(direction) {
			case Left:
				SetHeight(Container->GetUsableHeight()+margins.TotalY());
				Move(-margins.Left, -margins.Top);
				break;
			case Top:
				SetWidth(Container->GetUsableWidth()+margins.TotalX());
				Move(-margins.Left, -margins.Top);
				break;
			case Right:
				SetHeight(Container->GetUsableHeight()+margins.TotalY());
				Move(Container->GetUsableWidth()+margins.Right-GetWidth(), -margins.Top);
				break;
			case Bottom:
				SetWidth(Container->GetUsableWidth()+margins.TotalX());
				Move(-margins.Left, Container->GetUsableHeight()+margins.Bottom-GetHeight());
				break;
			}
		}

		utils::Property<Panel, utils::Margins> Padding;

	protected:
		void setPadding(const utils::Margins &value) {
			Base::setpadding(value);
		}
		utils::Margins getPadding() const {
			return Base::getpadding();
		}
		void wr_loaded() {
			if(!blueprintmodified && WR.Panel)
				setblueprint(*WR.Panel);
		}
	};

}}
