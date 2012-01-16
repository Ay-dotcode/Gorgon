#pragma once

#include "../DialogWindow.h"
#include "../Label.h"

namespace gge { namespace widgets {

	class MessageDialog : public DialogWindow {
	public:
		enum DialogIcon {
			None	= 0,
			Success	= 1,
			Error	= 2,
			Warning = 3,
			Canceled= 4,
		};

		MessageDialog() : INIT_PROPERTY(MessageDialog, Message)
		{
			init();
		}

		template<class T_>
		MessageDialog(const T_ &msg) : INIT_PROPERTY(MessageDialog, Message) {
			init();
			Message=msg;
		}

		template<class T_>
		MessageDialog(const T_ &msg, std::string icon) : INIT_PROPERTY(MessageDialog, Message) {
			init();
			Message=msg;
			SetIcon(icon);
		}

		template<class T_>
		MessageDialog(const T_ &msg, DialogIcon icon) : INIT_PROPERTY(MessageDialog, Message) {
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

		void SetIcon(std::string icon);
		void SetIcon(DialogIcon icon);

		virtual void MoveToCenter() {
			DialogWindow::MoveToCenter();
			autocenter=true;
		}

		utils::TextualProperty<MessageDialog> Message;

		~MessageDialog();

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
			ok.Accesskey='o';
			this->SetDefault(ok);
			this->SetCancel(ok);

			ok.ClickEvent().RegisterLambda([&]{Close();});

			MoveToCenter();
		}

		void resize();

		void setMessage(const std::string &value) {
			if(message.Text!=value) {
				message.Text=value;
				resize();
			}
		}
		std::string getMessage() const {
			return message.Text;
		}
	};

	MessageDialog &ShowMessage(string Message, string Title="");

}}
