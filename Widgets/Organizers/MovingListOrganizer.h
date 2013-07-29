#pragma once


#include "../Base/Organizer.h"

namespace gge { namespace widgets {

	class MovingListOrganizer : public Organizer {
	public:

		MovingListOrganizer() : distance(0), columns(1), currentscroll(0), logicalcount(0), topitem(0), working(false) {

		}

		virtual void Reorganize() {
			if(!attachedto) return;
			if(attachedto->Widgets.GetCount()<1) return;
			if(working) return;

			working=true;

			int integralheight=attachedto->Widgets.First()->GetHeight();
			topitem=int(std::floor((currentscroll+distance)/(integralheight+distance)))*columns;
			int itemnum=topitem;
			int y=int(std::ceil(float(topitem)/columns))*integralheight+(topitem-1)/columns*distance;
			int lasty=currentscroll+attachedto->GetUsableHeight();
			int w=attachedto->GetUsableWidth()/columns;
			int ind=0;
			for(auto it=attachedto->Widgets.First();it.IsValid();it.Next()) {
				if(itemnum>=logicalcount || y>=lasty) {
					it->Hide();
				}
				else {
					it->Show();
					it->SetWidth(w -  ( (ind%columns)==columns-1  ? 0 : distance ));
					it->Move((ind%columns)*w, y);
					if((ind%columns)==columns-1)
						y+=it->GetHeight()+distance;
				}
				ind++;
				itemnum++;
			}

			working=false;
		}

		void SetLogicalCount(int count) {
			if(!attachedto) {
				logicalcount=count;
				return;
			}
			if(count>logicalcount) {
				logicalcount=count;
				if(logicalcount-topitem<attachedto->Widgets.GetCount()) {
					Reorganize();
				}
			}
			else if(count<logicalcount) {
				logicalcount=count;
				if(count-topitem<attachedto->Widgets.GetCount()) {
					Reorganize();
				}
			}
		}

		int RequiredItems(int height) {
			if(!attachedto) return 0;

			int h=attachedto->GetHeight();

			return (int)std::ceil(float(h-distance)/(height+distance))*columns;
		}

		int GetLogicalHeight() {
			if(!attachedto) return 0;

			if(attachedto->Widgets.GetCount()<1) return 0;

			int height=attachedto->Widgets.First()->GetHeight();

			return int(std::ceil(logicalcount/columns))*(height+distance)-distance;
		}

		gge::utils::Point GetLogicalLocation(int index) {
			if(!attachedto) return gge::utils::Point(0,0);

			if(attachedto->Widgets.GetCount()<1) return gge::utils::Point(0,0);

			return gge::utils::Point(
				(index%columns)*(attachedto->Widgets.First()->GetWidth()+distance),
				(index/columns)*(attachedto->Widgets.First()->GetHeight()+distance)
			);
		}

		//Returns if the control needs reorganizing
		bool SetVScroll(int scroll) {
			currentscroll=scroll;
			if(!attachedto) return false;

			if(attachedto->Widgets.GetCount()<1) return false;

			bool reorganize=false;
			if(scroll>attachedto->Widgets.First()->GetY()) {
				reorganize=true;
			}
			if(scroll<attachedto->Widgets.First()->GetBounds().BottomRight().y) {
				reorganize=true;
			}

			if(reorganize) {
				Reorganize();
			}

			return reorganize;
		}

		int GetTop() {
			return topitem;
		}

		void SetDistance(int value) {
			distance=value;
			Reorganize();
		}

		void SetColumns(int value) {
			columns=value;
			Reorganize();
		}

		int LogicalItemAt(gge::utils::Point location, bool wrapx=false) {
			if(!attachedto) return 0;

			if(attachedto->Widgets.GetCount()<1) return 0;

			int height=attachedto->Widgets.First()->GetHeight();
			int width=attachedto->Widgets.First()->GetWidth();

			int index=(location.y+distance)/(height+distance)*columns;
			index+=std::min((location.x+distance)/(width+distance),columns+(wrapx?1:0));
			
			if(index>logicalcount+1) index=logicalcount+1;
			if(index<-1) index=-1;

			return index;
		}

	protected:
		int distance;
		int columns;
		int currentscroll;
		int logicalcount;
		int topitem;
		bool working;
	};


}}
