#pragma once

#include "../DialogWindow.h"
#include "../Label.h"
#include "../Textbox.h"

namespace gge { namespace widgets { namespace dialog {

	class TextInput;

	TextInput &AskText(const std::string &Message, const std::string Default="", const std::string &Title="");

	class TextInput : public DialogWindow {
		friend TextInput &AskText(const std::string &msg, const std::string Default, const std::string &Title);
	public:

		TextInput() : INIT_PROPERTY(TextInput, MessageText), 
			INIT_PROPERTY(TextInput, OkButtonText), INIT_PROPERTY(TextInput, CancelButtonText),
			INIT_PROPERTY(TextInput, InputText)
		{
			init();
		}

		template<class T_>
		TextInput(const T_ &msg) : INIT_PROPERTY(TextInput, MessageText), 
			INIT_PROPERTY(TextInput, OkButtonText), INIT_PROPERTY(TextInput, CancelButtonText),
			INIT_PROPERTY(TextInput, InputText)
		{
			init();
			InputText=msg;
		}

		template<class T_>
		TextInput(const T_ &msg, const std::string &icon) : INIT_PROPERTY(TextInput, MessageText), 
			INIT_PROPERTY(TextInput, OkButtonText), INIT_PROPERTY(TextInput, CancelButtonText),
			INIT_PROPERTY(TextInput, InputText)
		{
			init();
			InputText=msg;
			SetIcon(icon);
		}

		virtual void Resize(utils::Size size) {
			if(size==WidgetBase::size) return;
			if(size.Height!=WidgetBase::size.Height)
				autosize=false;

			DialogWindow::Resize(size);
			message.SetWidth(GetUsableWidth());
			input.SetWidth(GetUsableWidth());
			resize();
			message.SetWidth(GetUsableWidth());
			input.SetWidth(GetUsableWidth());
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
			input.SetBlueprint(WR.Textbox);
		}

		TextInput &SetIcon(const std::string &icon);
		void RemoveIcon();

		virtual void MoveToCenter() {
			DialogWindow::MoveToCenter();
			autocenter=true;
		}

		utils::TextualProperty<TextInput> MessageText;
		utils::TextualProperty<TextInput> OkButtonText;
		utils::TextualProperty<TextInput> CancelButtonText;
		utils::TextualProperty<TextInput> InputText;

		~TextInput();

		utils::EventChain<TextInput, const std::string&> RepliedEvent;

	protected:
		Label message;
		Button ok,cancel;
		Textbox input;
		bool iconowner;
		bool autosize;
		bool autocenter;

		void init()  {
			if(activevp)
				SetContainer(activevp);
			else
				SetContainer(TopLevel);
			SetBlueprint(WR.Panels.DialogWindow);
			iconowner=false;
			autosize=true;
			autocenter=true;

			message.TextWrap=true;
			message.SetContainer(this);
			message.SetWidth(this->GetUsableWidth());
			message.SetBlueprint(WR.Label);

			SetIcon("question");

			input.SetContainer(this);
			input.SetWidth(this->GetUsableWidth());
			input.SetBlueprint(WR.Textbox);
			input.AutoSelectAll=true;
			input.Focus();

			dialogbuttons.Add(cancel);
			placedialogbutton(cancel);
			cancel.Text="Cancel";
			this->SetCancel(cancel);
			cancel.Autosize=AutosizeModes::GrowOnly;

			dialogbuttons.Add(ok);
			placedialogbutton(ok);
			ok.Text="OK";
			this->SetDefault(ok);
			ok.Autosize=AutosizeModes::GrowOnly;

			cancel.ClickEvent.RegisterLambda([&]{ Close(); });
			ok.ClickEvent.RegisterLambda([&]{ RepliedEvent(this->input.Text); Close(); });

			MoveToCenter();
		}

		void resize();

		void reset() {
			if(activevp)
				SetContainer(activevp);
			else
				SetContainer(TopLevel);
			MessageText="";
			InputText="";
			Title="";

			SetIcon("question");
			MoveToCenter();
			if(bp)
				SetWidth(bp->DefaultSize.Width);
			Autosize();

			ClosingEvent.Clear();
			RollUpEvent.Clear();
			RollDownEvent.Clear();
			RepliedEvent.Clear();

			autosize=true;
			autocenter=true;
			ok.Text="OK";
			cancel.Text="Cancel";
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

		void setOkButtonText(const std::string &value) {
			ok.Text=value;
		}
		std::string getOkButtonText() const {
			return ok.Text;
		}

		void setCancelButtonText(const std::string &value) {
			cancel.Text = value;
		}
		std::string getCancelButtonText() const {
			return cancel.Text;
		}

		void setInputText(const std::string &value) {
			input.Text = value;
		}
		std::string getInputText() const {
			return input.Text;
		}
	};

}}}
