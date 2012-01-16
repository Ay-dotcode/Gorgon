#include "MessageDialog.h"

namespace gge { namespace widgets {

	char *dialogicons[] = {"","success","error","warning","canceled"};
	std::vector<MessageDialog*> messagedialogs;




	void MessageDialog::SetIcon(std::string icon) {
		if(Icon!=NULL && iconowner && dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr()))
			dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr())->DeleteAnimation();

		Icon=WR.Icons(icon);
		iconowner=true;
	}

	void MessageDialog::SetIcon(DialogIcon icon) {
		if(Icon!=NULL && iconowner && dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr()))
			dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr())->DeleteAnimation();

		Icon=WR.Icons(dialogicons[icon]);
		iconowner=true;
	}

	MessageDialog::~MessageDialog() {
		if(iconowner && dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr()))
			dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr())->DeleteAnimation();
	}

	void MessageDialog::resize() {
		if(GetUsableWidth()>0 && autosize) {
			SetHeight(message.GetHeight()+GetHeight()-GetUsableHeight()+(*Padding).Bottom);

			autosize=true;

			if(autocenter)
				MoveToCenter();
		}
	}

	MessageDialog &ShowMessage(string Message, string Title) {
		MessageDialog *m=NULL;
		for(auto it=messagedialogs.begin();it!=messagedialogs.end();++it) {
			if(!(*it)->IsVisible()) {
				m=*it;
			}
		}

		if(!m) {
			m=new MessageDialog;
		}

		m->Title=Title;
		m->Message=Message;
		m->Show(true);

		return *m;
	}

}}