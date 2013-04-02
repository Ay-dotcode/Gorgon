#pragma once

#include "../DialogWindow.h"
#include "../Label.h"
#include "../Numberbox.h"

namespace gge { namespace widgets { namespace dialog {

	template<class T_>
	class NumberInput;

	template<class T_>
	NumberInput<T_> &AskNumber(const std::string &Message, const T_ Default="", const std::string &Title="");

	template<class T_>
	class NumberInput : public DialogWindow {
		friend NumberInput &AskNumber<T_>(const std::string &msg, const T_ Default, const std::string &Title);
	public:

		NumberInput() : INIT_PROPERTY(NumberInput, MessageText), 
			INIT_PROPERTY(NumberInput, OkButtonText), INIT_PROPERTY(NumberInput, CancelButtonText),
			INIT_PROPERTY(NumberInput, InputNumber)
		{
			init();
		}

		template<class T_>
		NumberInput(const T_ &msg) : INIT_PROPERTY(NumberInput, MessageText), 
			INIT_PROPERTY(NumberInput, OkButtonText), INIT_PROPERTY(NumberInput, CancelButtonText),
			INIT_PROPERTY(NumberInput, InputNumber)
		{
			init();
			InputNumber=msg;
		}

		template<class T_>
		NumberInput(const T_ &msg, const std::string &icon) : INIT_PROPERTY(NumberInput, MessageText), 
			INIT_PROPERTY(NumberInput, OkButtonText), INIT_PROPERTY(NumberInput, CancelButtonText),
			INIT_PROPERTY(NumberInput, InputNumber)
		{
			init();
			InputNumber=msg;
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

		NumberInput &SetIcon(const std::string &icon) {
			if(Icon!=NULL && iconowner && dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr()))
				dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr())->DeleteAnimation();

			Icon=WR.Icons(icon);
			iconowner=true;

			return *this;
		}

		void RemoveIcon() {
			if(Icon!=NULL && iconowner && dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr()))
				dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr())->DeleteAnimation();
			Icon=NULL;
		}

		virtual void MoveToCenter() {
			DialogWindow::MoveToCenter();
			autocenter=true;
		}

		utils::TextualProperty<NumberInput> MessageText;
		utils::TextualProperty<NumberInput> OkButtonText;
		utils::TextualProperty<NumberInput> CancelButtonText;
		utils::NumericProperty<NumberInput, T_> InputNumber;

		~NumberInput() {
			if(iconowner && dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr()))
				dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr())->DeleteAnimation();
		}

		utils::EventChain<NumberInput, T_> RepliedEvent;

	protected:
		Label message;
		Button ok,cancel;
		Numberbox<T_> input;
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
			ok.ClickEvent.RegisterLambda([&]{ RepliedEvent(this->input.Value); Close(); });

			MoveToCenter();
		}

		void resize() {
			if(GetUsableWidth()>0 && autosize) {
				SetHeight(Main.BoundingBox.Height());
				SetHeight(message.GetHeight()+(*Padding).Bottom+input.GetHeight()+GetHeight()-GetUsableHeight()+(*Padding).Bottom);
				input.SetY(message.GetHeight()+(*Padding).Bottom);

				autosize=true;

				if(autocenter)
					MoveToCenter();
			}
		}

		void reset() {
			if(activevp)
				SetContainer(activevp);
			else
				SetContainer(TopLevel);
			MessageText="";
			InputNumber=0;
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

		void setInputNumber(const T_ &value) {
			input.Value = value;
		}
		T_ getInputNumber() const {
			return input.Value;
		}
	};

	template<class T_>
	NumberInput<T_> &AskNumber(const std::string &msg, T_ Default, const std::string &Title) {
		static std::vector<NumberInput<T_>*> NumberInputs;

		NumberInput<T_> *m=NULL;
		for(auto it=NumberInputs.begin();it!=NumberInputs.end();++it) {
			if(!(*it)->IsVisible()) {
				m=*it;
				break;
			}
		}

		if(!m) {
			m=new NumberInput<T_>;
			NumberInputs.push_back(m);
		}

		m->reset();
		m->Title=Title;
		m->MessageText=msg;
		m->InputNumber=Default;
		m->Show(true);

		return *m;
	}

}}}
