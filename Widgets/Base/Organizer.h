#pragma once


#include "Widget.h"

namespace gge { namespace widgets {

	class Organizer {
		friend class ContainerBase;
	public:

		virtual void Reorganize() = 0;

	protected:
		virtual void SetAttached(ContainerBase *attached);

	private:
		ContainerBase *attachedto;
	};

}}
