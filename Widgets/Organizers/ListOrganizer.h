#pragma once


#include "../Base/Organizer.h"

namespace gge { namespace widgets {

	class ListOrganizer : public Organizer {
	public:

		virtual void Reorganize() {
			if(!attachedto) return;

			int y=0;
			for(auto it=attachedto->Widgets.First();it.IsValid();it.Next()) {
				if(it->IsVisible()) {
					it->SetWidth(attachedto->GetUsableWidth());
					it->Move(0, y);
					y+=it->GetHeight();
				}
			}

		}

	};


}}
