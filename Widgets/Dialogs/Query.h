#pragma once

#include "../DialogWindow.h"
#include "../Label.h"
#include <cctype>

namespace gge { namespace widgets { namespace dialog {

	class Query : public DialogWindow {
	public:

		Query() : INIT_PROPERTY(Query, QueryText), INIT_PROPERTY(Query, YesButtonText), INIT_PROPERTY(Query, NoButtonText)
		{
			init();
		}

		template<class T_>
		Query(const T_ &msg) : INIT_PROPERTY(Query, QueryText), INIT_PROPERTY(Query, YesButtonText), INIT_PROPERTY(Query, NoButtonText) {
			init();
			Query=msg;
		}

		template<class T_>
		Query(const T_ &msg, std::string icon) : INIT_PROPERTY(Query, QueryText), INIT_PROPERTY(Query, YesButtonText), INIT_PROPERTY(Query, NoButtonText) {
			init();
			Query=msg;
			SetIcon(icon);
		}

		virtual void Resize(utils::Size size) {
			if(size==WidgetBase::size) return;
			if(size.Height!=WidgetBase::size.Height)
				autosize=false;

			DialogWindow::Resize(size);
			query.SetWidth(GetUsableWidth());
			resize();
			query.SetWidth(GetUsableWidth());
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
			query.SetBlueprint(WR.Label);
		}

		void SetIcon(const std::string &icon);
		void RemoveIcon();

		virtual void MoveToCenter() {
			DialogWindow::MoveToCenter();
			autocenter=true;
		}

		utils::TextualProperty<Query> QueryText;
		utils::TextualProperty<Query> YesButtonText;
		utils::TextualProperty<Query> NoButtonText;

		~Query();

		utils::EventChain<Query, bool> RepliedEvent;

	protected:
		Label query;
		Button yes,no;
		bool iconowner;
		bool autosize;
		bool autocenter;

		void init()  {
			SetBlueprint(WR.Panels.DialogWindow);
			iconowner=false;
			autosize=true;
			autocenter=true;
			query.TextWrap=true;
			query.SetContainer(this);
			query.SetWidth(this->GetUsableWidth());
			query.SetBlueprint(WR.Label);

			SetIcon("question");

			dialogbuttons.Add(yes);
			placedialogbutton(yes);
			yes.Text="Yes";
			yes.Accesskey='y';
			this->SetDefault(yes);
			yes.ClickEvent().RegisterLambda([&]{ RepliedEvent(true); Close();});


			dialogbuttons.Add(no);
			placedialogbutton(no);
			no.Text="No";
			no.Accesskey='n';
			this->SetCancel(no);
			no.ClickEvent().RegisterLambda([&]{ RepliedEvent(false); Close();});


			MoveToCenter();
		}

		void resize();

		void setQueryText(const std::string &value) {
			if(query.Text!=value) {
				query.Text=value;
				resize();
			}
		}
		std::string getQueryText() const {
			return query.Text;
		}

		void setYesButtonText(const std::string &value) {
			yes.Text=value;
			yes.Accesskey=std::tolower(value[0]);
		}
		std::string getYesButtonText() const {
			return yes.Text;
		}

		void setNoButtonText(const std::string &value) {
			no.Text=value;
			no.Accesskey=std::tolower(value[0]);
		}
		std::string getNoButtonText() const {
			return no.Text;
		}
	};

	Query &QueryUser(const string &Confirm, const string &Title="");

}}}
