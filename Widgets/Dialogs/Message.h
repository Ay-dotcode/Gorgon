#pragma once

#include "../DialogWindow.h"
#include "../Label.h"

namespace gge { namespace widgets { namespace dialog {

	class Message;

	Message &ShowMessage(const std::string &Message, const std::string &Title="");

	class Message : public DialogWindow {
		friend Message &ShowMessage(const string &msg, const string &Title);
	public:
		enum DialogIcon {
			None	= 0,
			Success	= 1,
			Error	= 2,
			Warning = 3,
			Canceled= 4,
		};

		Message() : INIT_PROPERTY(Message, MessageText), INIT_PROPERTY(Message, ButtonText)
		{
			init();
		}

		template<class T_>
		Message(const T_ &msg) : INIT_PROPERTY(Message, MessageText), INIT_PROPERTY(Message, ButtonText) {
			init();
			Message=msg;
		}

		template<class T_>
		Message(const T_ &msg, const std::string &icon) : INIT_PROPERTY(Message, MessageText), INIT_PROPERTY(Message, ButtonText) {
			init();
			Message=msg;
			SetIcon(icon);
		}

		template<class T_>
		Message(const T_ &msg, DialogIcon icon) : INIT_PROPERTY(Message, MessageText), INIT_PROPERTY(Message, ButtonText) {
			init();
			Message=msg;
			SetIcon(icon);
		}

		virtual void Resize(utils::Size size) {
			if(size==WidgetBase::size) return;
			if(size.Height!=WidgetBase::size.Height)
				autosize=false;

			DialogWindow::Resize(size);
			message.SetWidth(GetUsableWidth());
			resize();
			message.SetWidth(GetUsableWidth());
			resize();

			if(autocenter)
				MoveToCenter();
		}

		virtual void Move(utils::Point location) {
			if(location!=GetLocation()) {
				DialogWindow::Move(location);
				autocenter=false;
			}
		}

		virtual void ForcedClose() {
			DialogWindow::ForcedClose();
		}

		void Autosize() {
			autosize=true;
			resize();
		}

		using DialogWindow::SetBlueprint;
		virtual void SetBlueprint(const widgets::Blueprint &bp) {
			DialogWindow::SetBlueprint(bp);
			message.SetBlueprint(WR.Label);
		}

		Message &SetIcon(const std::string &icon);
		Message &SetIcon(DialogIcon icon);
		void RemoveIcon();

		virtual void MoveToCenter() {
			DialogWindow::MoveToCenter();
			autocenter=true;
		}

		utils::TextualProperty<Message> MessageText;
		utils::TextualProperty<Message> ButtonText;

		~Message();

	protected:
		Label message;
		Button ok;
		bool iconowner;
		bool autosize;
		bool autocenter;

		void init()  {
			SetBlueprint(WR.Panels.DialogWindow);
			iconowner=false;
			autosize=true;
			autocenter=true;
			message.TextWrap=true;
			message.SetContainer(this);
			message.SetWidth(this->GetUsableWidth());
			message.SetBlueprint(WR.Label);

			dialogbuttons.Add(ok);
			placedialogbutton(ok);
			ok.Text="OK";
			this->SetDefault(ok);
			this->SetCancel(ok);
			ok.Autosize=AutosizeModes::GrowOnly;

			ok.ClickEvent().RegisterLambda([&]{Close();});

			MoveToCenter();
		}

		void resize();

		void reset() {
			MessageText="";
			Title="";

			ok.RemoveFocus();

			RemoveIcon();
			MoveToCenter();
			if(bp)
				SetWidth(bp->DefaultSize.Width);
			Autosize();

			ClosingEvent.Clear();
			RollUpEvent.Clear();
			RollDownEvent.Clear();

			autosize=true;
			autocenter=true;
			ok.Text="OK";
		}

		void setMessageText(const std::string &value) {
			if(message.Text!=value) {
				message.Text=value;
				resize();
			}
		}
		std::string getMessageText() const {
			return message.Text;
		}

		void setButtonText(const std::string &value) {
			ok.Text=value;
		}
		std::string getButtonText() const {
			return ok.Text;
		}
	};

}}}
