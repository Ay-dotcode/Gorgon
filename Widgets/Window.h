#pragma once

#include "WidgetRegistry.h"
#include "Panel/PanelBase.h"
#include "Button.h"
#include "Main.h"

namespace gge { namespace widgets {

	class Window : public panel::Base {
	public:

		Window() : INIT_PROPERTY(Window, Padding),
			INIT_PROPERTY(Window, Title),
			INIT_PROPERTY(Window, Icon),
			INIT_PROPERTY(Window, CloseButton),
			INIT_PROPERTY(Window, RollButton),
			ClosingEvent("Closing", this),
			RollUpEvent("Roll up", this),
			RollDownEvent("Roll down", this),
			rolled(false)
		{
			setallowmove(true);
			setallownofocus(true);
			setshowtitle(true);
			setupvscroll(true, true, true);
			GotFocus.Register(this, &Window::gotfocus);

			rollbtn.ClickEvent().Register(this, &Window::ToggleRoll);
			titlebuttons.Add(rollbtn);
			placetitlebutton(rollbtn);
			rollbtn.Hide();

			closebtn.ClickEvent().Register(this, &Window::Close);
			closebtn.Text="x";
			titlebuttons.Add(closebtn);
			placetitlebutton(closebtn);

			Title=" ";
			SetContainer(TopLevel);

			if(WR.Window)
				setblueprint(*WR.Window);
		}

		~Window() {
			dynamic_cast<animation::Base*>(closebtn.Icon.GetPtr())->DeleteAnimation();
		}

		void Close() {
			bool allow=true;
			ClosingEvent(allow);

			if(allow)
				ForcedClose();
		}
		virtual void ForcedClose() {
			Hide();
		}

		void RollUp() {
			bool allow=true,
			RollUpEvent(allow);

			if(allow)
				ForcedRollUp();
		}

		virtual void ForcedRollUp() {
			prevh=GetHeight();

			int h=title.GetHeight()+(GetHeight()-GetUsableHeight()-title.GetHeight()-getcontrolmargins().Bottom)/2;
			if(getinnerborder() && getinnerborder()->Margins.TotalY()<0) {
				h-=getinnerborder()->Margins.TotalY();
			}
			SetHeight(h);
			setupvscroll(false, false, false);
			innerlayer.isVisible=false;
			rolled=true;
		}
		void RollDown() {
			bool allow=true,
				RollDownEvent(allow);

			if(allow)
				ForcedRollDown();
		}
		virtual void ForcedRollDown() {
			SetHeight(prevh);
			setupvscroll(true, true, true);
			innerlayer.isVisible=true;
			rolled=false;
		}
		void ToggleRoll() {
			rolled ? RollDown() : RollUp();
		}
		void SetRoll(bool rolled) {
			rolled ? RollUp() : RollDown();
		}
		bool IsRolledUp() {
			return rolled;
		}



		utils::Property<Window, utils::Margins> Padding;
		utils::TextualProperty<Window> Title;
		utils::ReferenceProperty<Window, graphics::RectangularGraphic2D> Icon;
		utils::BooleanProperty<Window> CloseButton;
		utils::BooleanProperty<Window> RollButton;

		utils::EventChain<Window, bool&> ClosingEvent;
		utils::EventChain<Window, bool&> RollUpEvent;
		utils::EventChain<Window, bool&> RollDownEvent;

		virtual void MoveToCenter()  {
			if(Container) {
				utils::Size sz=Container->GetUsableSize();
				sz=(sz-WidgetBase::size)/2;
				Move(sz.Width, sz.Height);
			}
		}

	protected:
		Button closebtn;
		Button rollbtn;
		bool rolled;
		int prevh;

		virtual void setblueprint(const widgets::Blueprint &bp) {
			Base::setblueprint(bp);
			closebtn.Icon=WR.Icons("window-close");
			rollbtn.Icon=WR.Icons("window-roll");
		}

		void gotfocus() {
			WidgetBase::ToTop();
			FocusOrderToTop();
		}

		void setPadding(const utils::Margins &value) {
			Base::setpadding(value);
		}
		utils::Margins getPadding() const {
			return Base::getpadding();
		}

		void setTitle(const std::string &value) {
			Base::settitle(value);
		}
		std::string getTitle() const {
			return Base::gettitle();
		}

		void setIcon(graphics::RectangularGraphic2D *icon) {
			Base::seticon(icon);
		}
		graphics::RectangularGraphic2D *getIcon() const {
			return Base::geticon();
		}

		void setCloseButton(const bool &value) {
			if(closebtn.IsVisible()!=value) {
				closebtn.SetVisibility(value);
				adjustcontrols();
			}
		}
		bool getCloseButton() const {
			return closebtn.IsVisible();
		}

		void setRollButton(const bool &value) {
			if(rollbtn.IsVisible()!=value) {
				rollbtn.SetVisibility(value);
				adjustcontrols();
			}
		}
		bool getRollButton() const {
			return rollbtn.IsVisible();
		}
		void wr_loaded() {
			if(!blueprintmodified && WR.Window)
				setblueprint(*WR.Window);
		}


	};

}}
