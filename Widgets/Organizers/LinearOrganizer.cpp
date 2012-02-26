#include "LinearOrganizer.h"
#include "../Base/Container.h"
#include "../Slider.h"
#include "../Combobox/ComboboxBase.h"
#include "../Panel.h"
#include "../Textbox/TextboxBase.h"
#include "../Listbox/ListboxBase.h"


namespace gge { namespace widgets {






	void LinearOrganizer::Reorganize() {
		if(!attachedto) return;

		sync();

		std::map<int, int> colmaxw;

		for(auto row=rows.begin();row!=rows.end();++row) {
			int col=0;
			for(auto w=row->columns.begin();w!=row->columns.end();++w) {
				WidgetBase *widget=*w;

				if(colmaxw[col]<widget->GetWidth() && 
					(dynamic_cast<Label*>(widget)) 
					) {

					colmaxw[col]=widget->GetWidth();
				}

				col++;
			}
		}

		int y=0;
		for(auto row=rows.begin();row!=rows.end();++row) {
			int maxh=0;
			int maxsizedwidgets=0;
			int variablesize=0;
			int carry=0;
			int fixedwidth=0;
			
			int col=0;
			for(auto w=row->columns.begin();w!=row->columns.end();++w) {
				WidgetBase *widget=*w;

				if(
					(dynamic_cast<ISliderType*>(widget) && !dynamic_cast<ISliderType*>(widget)->IsVertical()) ||
					(dynamic_cast<textbox::Base*>(widget)) ||
					(dynamic_cast<ComboboxType*>(widget)) ||
					(dynamic_cast<Panel*>(widget)) ||
					(dynamic_cast<ListboxType*>(widget))
				) {
					maxsizedwidgets++;
				}
				else {
					if(colmaxw[col]>0 && dynamic_cast<Label*>(widget))
						fixedwidth+=colmaxw[col]+spacing;
					else
						fixedwidth+=widget->GetWidth()+spacing;
				}

				col++;
			}

			if(maxsizedwidgets) {
				variablesize=(attachedto->GetUsableWidth()-fixedwidth-maxsizedwidgets*spacing)/maxsizedwidgets;
				carry=(attachedto->GetUsableWidth()-fixedwidth-maxsizedwidgets*spacing)-variablesize*maxsizedwidgets;
			}

			int x=0;
			col=0;
			for(auto w=row->columns.begin();w!=row->columns.end();++w) {
				WidgetBase *widget=*w;
				if(widget->IsVisible()) {
					widget->Move(x,y);

					if(
						(dynamic_cast<ISliderType*>(widget) && !dynamic_cast<ISliderType*>(widget)->IsVertical()) ||
						(dynamic_cast<textbox::Base*>(widget)) ||
						(dynamic_cast<ComboboxType*>(widget)) ||
						(dynamic_cast<Panel*>(widget)) ||
						(dynamic_cast<ListboxType*>(widget))
						) {

						widget->SetWidth(variablesize+carry);
						carry=0;

						x+=widget->GetWidth()+spacing;
					}
					else if(colmaxw[col]>0 && dynamic_cast<Label*>(widget))
						x+=colmaxw[col]+spacing;
					else
						x+=widget->GetWidth()+spacing;

					maxh=std::max(maxh, widget->GetHeight());
				}

				col++;
			}

			if(row->height==-1) {
				y+=maxh+spacing;
			}
			else {
				y+=row->height;
			}
		}
	}

	void LinearOrganizer::sync() {
		if(!attachedto) return;

		for(auto w=organizedwidgets.First();w.isValid();w.Next()) {
			if(!attachedto->Widgets.Find(*w).isValid()) {
				w.Remove();
			}
		}

		for(auto w=attachedto->Widgets.First();w.isValid();w.Next()) {
			if(!organizedwidgets.Find(*w).isValid()) {
				organizedwidgets.Add(w);
				rows[currentrow].columns.push_back(w.CurrentPtr());
			}
		}
	}

	LinearOrganizer::Modifier LinearOrganizer::endl=&LinearOrganizer::nextline_;
	LinearOrganizer::Modifier LinearOrganizer::nextline=&LinearOrganizer::nextline_;

}}
