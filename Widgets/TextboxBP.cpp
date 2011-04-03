#include "TextboxBP.h"

namespace gge { namespace widgets {

	TextboxBP::TextboxBP(void) {
		TypingSound=NULL;
	}

	IWidgetObject *TextboxBP::Create(IWidgetContainer &Container,int X,int Y,int Cx,int Cy) {
		Textbox *txt=new Textbox(this,Container);
		txt->PointerType=PointerType;
		txt->Resize(Cx,Cy);
		txt->Move(X,Y);
		
		return txt;
	}

	void TextboxBP::Prepare(gge::GGEMain *main) {
		ResourceBase::Prepare(main);
		if(!Caret->Subitems.getCount())
			Caret=NULL;

		TypingSound=(SoundResource*)file->FindObject(guid_typingsound);
	}
} }
