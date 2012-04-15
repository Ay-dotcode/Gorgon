#pragma once

#include "Panel/PanelBase.h"

namespace gge { namespace widgets {

	class FullscreenPanel : public panel::Base {
	public:

		FullscreenPanel() : INIT_PROPERTY(FullscreenPanel, Padding)
		{
			setallowmove(true);
			setupvscroll(true, true, true);

			if(WR.Panel)
				setblueprint(*WR.Panels.Fullscreen);

			SetContainer(TopLevel);
			Resize(gge::Main.BoundingBox.GetSize());
		}

		void FillContainer() {
			if(Container) {
				Move(0,0);
				Resize(Container->GetUsableWidth()-1, Container->GetUsableHeight()-1);
			}
		}

		using WidgetBase::Resize;

		virtual void Resize(utils::Size Size) {
			utils::Rectangle r=os::window::UsableScreenMetrics();

			if(Size.Width>r.Width)
				Size.Width=r.Width;
			if(Size.Height>r.Height)
				Size.Height=r.Height;

			Main.ResizeWindow(Size);

			Move(r.Left+(r.Width-Size.Width)/2, r.Top+(r.Height-Size.Height)/4);
			Draw();
		}

		using WidgetBase::GetSize;

		virtual utils::Size GetSize() {
			return gge::Main.BoundingBox.GetSize();
		}

		using WidgetBase::Move;

		virtual void Move(utils::Point Location) {
			gge::Main.MoveWindow(Location);
			location=Location;
		}

		utils::Property<FullscreenPanel, utils::Margins> Padding;

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
