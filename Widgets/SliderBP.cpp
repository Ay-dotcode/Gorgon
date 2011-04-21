#include "SliderBP.h"
#include <assert.h>

namespace gge { namespace widgets {

	SliderBP::SliderBP(void) {
		TickSound=NULL;
	}

	IWidgetObject *SliderBP::Create(IWidgetContainer &Container,int X,int Y,int Cx,int Cy) {
		/*Slider *slider=new Slider(this,Container);
		slider->PointerType=PointerType;
		slider->Resize(Cx,Cy);
		slider->Move(X,Y);*/
		
		assert(0); // YOU CANNOT CREATE A SLIDER WITH THIS METHOD

		return NULL;
	}

	void SliderBP::Prepare(gge::GGEMain *main) {
		ResourceBase::Prepare(main);

		TickSound=dynamic_cast<resource::SoundResource*>(file->FindObject(guid_ticksound));
	}
} }

