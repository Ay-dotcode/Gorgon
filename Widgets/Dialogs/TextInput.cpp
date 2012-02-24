#include "TextInput.h"

using std::string;

namespace gge { namespace widgets { namespace dialog {

	std::vector<TextInput*> TextInputs;




	TextInput &TextInput::SetIcon(const std::string &icon) {
		if(Icon!=NULL && iconowner && dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr()))
			dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr())->DeleteAnimation();

		Icon=WR.Icons(icon);
		iconowner=true;

		return *this;
	}

	void TextInput::RemoveIcon() {
		if(Icon!=NULL && iconowner && dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr()))
			dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr())->DeleteAnimation();
		Icon=NULL;
	}

	TextInput::~TextInput() {
		if(iconowner && dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr()))
			dynamic_cast<animation::RectangularGraphic2DAnimation*>(Icon.GetPtr())->DeleteAnimation();
	}

	void TextInput::resize() {
		if(GetUsableWidth()>0 && autosize) {
			SetHeight(Main.BoundingBox.Height());
			SetHeight(message.GetHeight()+(*Padding).Bottom+input.GetHeight()+GetHeight()-GetUsableHeight()+(*Padding).Bottom);
			input.SetY(message.GetHeight()+(*Padding).Bottom);

			autosize=true;

			if(autocenter)
				MoveToCenter();
		}
	}

	TextInput &AskText(const string &msg, const std::string Default, const string &Title) {
		TextInput *m=NULL;
		for(auto it=TextInputs.begin();it!=TextInputs.end();++it) {
			if(!(*it)->IsVisible()) {
				m=*it;
				break;
			}
		}

		if(!m) {
			m=new TextInput;
			TextInputs.push_back(m);
		}

		m->reset();
		m->Title=Title;
		m->MessageText=msg;
		m->InputText=Default;
		m->Show(true);

		return *m;
	}

}}}
