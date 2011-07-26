#include "IWidgetBluePrint.h"

namespace gge { namespace widgets {
	IWidgetObject *IWidgetBluePrint::Create(IWidgetContainer& Container,Point position,int Cx,int Cy) { 
		return Create(Container,position.x,position.y,Cx,Cy); 
	}
} }
