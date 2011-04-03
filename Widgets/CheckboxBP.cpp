#include "CheckboxBP.h"
#include "CheckboxBase.h"

namespace gge { namespace widgets {

	CheckboxBP::CheckboxBP(void) {
	}

	IWidgetObject *CheckboxBP::Create(IWidgetContainer &Container,int X,int Y,int Cx,int Cy) {
		CheckboxBase *chk=new CheckboxBase(this,Container,CT_Checkbox);
		chk->PointerType=PointerType;
		chk->Resize(Cx,Cy);
		chk->Move(X,Y);
		
		return chk;
	}
} }
