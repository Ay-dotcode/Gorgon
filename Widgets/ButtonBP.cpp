#include "ButtonBP.h"

namespace gorgonwidgets {

	ButtonBP::ButtonBP(void) {
	}

	IWidgetObject *ButtonBP::Create(IWidgetContainer &Container,int X,int Y,int Cx,int Cy) {
		Button *btn=new Button(this,Container);
		btn->PointerType=PointerType;
		btn->Resize(Cx,Cy);
		btn->Move(X,Y);
		
		return btn;
	}
}