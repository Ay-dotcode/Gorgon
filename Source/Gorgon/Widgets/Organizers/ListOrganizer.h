#pragma once


#include "../Base/Organizer.h"

namespace gge { namespace widgets {

	class ListOrganizer : public Organizer {
	public:

		ListOrganizer() : distance(0), columns(1) {

		}

		virtual void Reorganize() {
			if(!attachedto) return;

			int y=0;
			int w=attachedto->GetUsableWidth()/columns;
			int ind=0;
			for(auto it=attachedto->Widgets.First();it.IsValid();it.Next()) {
				if(it->IsVisible()) {
					it->SetWidth(w -  ( (ind%columns)==columns-1  ? 0 : distance ));
					it->Move((ind%columns)*w, y);
					if((ind%columns)==columns-1)
						y+=it->GetHeight()+distance;
					ind++;
				}
			}

		}

		void SetDistance(int value) {
			distance=value;
			Reorganize();
		}

		void SetColumns(int value) {
			columns=value;
			Reorganize();
		}

	protected:
		int distance;
		int columns;
	};


}}
