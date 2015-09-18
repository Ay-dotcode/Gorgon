#include "Organizer.h"
#include "Container.h"


namespace gge { namespace widgets {

	void Organizer::SetAttached(ContainerBase *attached) {
		if(attachedto)
			attachedto->RemoveOrganizer();
		attachedto=attached;
	}

}}
