#include "IWidgetBluePrint.h"

namespace gorgonwidgets {
	IWidgetObject *IWidgetBluePrint::Create(IWidgetContainer& Container,Point position,int Cx,int Cy) { 
		return Create(Container,position.x,position.y,Cx,Cy); 
	}
}