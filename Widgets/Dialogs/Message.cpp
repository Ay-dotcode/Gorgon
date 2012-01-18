#include "Message.h"

namespace gge { namespace widgets { namespace dialog {

	char *dialogicons[] = {"","success","error","warning","canceled"};
	std::vector<Message*> Messages;




	Message &Message::SetIcon(const std::string &icon) {
		if(Icon!=NULL && iconowner && dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr()))
			dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr())->DeleteAnimation();

		Icon=WR.Icons(icon);
		iconowner=true;

		return *this;
	}

	Message &Message::SetIcon(DialogIcon icon) {
		if(Icon!=NULL && iconowner && dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr()))
			dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr())->DeleteAnimation();

		Icon=WR.Icons(dialogicons[icon]);
		iconowner=true;

		return *this;
	}

	void Message::RemoveIcon() {
		if(Icon!=NULL && iconowner && dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr()))
			dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr())->DeleteAnimation();
		Icon=NULL;
	}

	Message::~Message() {
		if(iconowner && dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr()))
			dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr())->DeleteAnimation();
	}

	void Message::resize() {
		if(GetUsableWidth()>0 && autosize) {
			SetHeight(message.GetHeight()+GetHeight()-GetUsableHeight()+(*Padding).Bottom);

			autosize=true;

			if(autocenter)
				MoveToCenter();
		}
	}

	Message &ShowMessage(const string &msg, const string &Title) {
		Message *m=NULL;
		for(auto it=Messages.begin();it!=Messages.end();++it) {
			if(!(*it)->IsVisible()) {
				m=*it;
				break;
			}
		}

		if(!m) {
			m=new Message;
			Messages.push_back(m);
		}

		m->Title=Title;
		m->MessageText=msg;
		m->Show(true);
		m->RemoveIcon();
		m->MoveToCenter();
		m->Autosize();
		m->ClosingEvent.Clear();
		m->RollUpEvent.Clear();
		m->RollDownEvent.Clear();

		return *m;
	}

}}}
