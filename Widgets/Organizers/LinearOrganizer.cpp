#include "LinearOrganizer.h"
#include "../Base/Container.h"
#include "../Slider.h"
#include "../Combobox/ComboboxBase.h"
#include "../Panel.h"
#include "../Textbox/TextboxBase.h"
#include "../Listbox/ListboxBase.h"
#include "../Window.h"
#include "../FullscreenPanel.h"


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
					(dynamic_cast<Label*>(widget)) && row->columns.size()>1
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
					(dynamic_cast<ListboxType*>(widget)) || 
					(dynamic_cast<FullWidth*>(widget)) || 
					(dynamic_cast<Label*>(widget) && col==(int)row->columns.size()-1 && maxsizedwidgets==0)
				) {
					maxsizedwidgets++;
				}
				else {
					if(colmaxw[col]>0 && dynamic_cast<Label*>(widget) && row->columns.size()>1)
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

			for(auto w=row->columns.begin();w!=row->columns.end();++w) {
				WidgetBase *widget=*w;
				maxh=std::max(maxh, widget->GetHeight());
			}

			int x=0;
			col=0;
			bool allbuttons=true;
			for(auto w=row->columns.begin();w!=row->columns.end();++w) {
				WidgetBase *widget=*w;
				if(widget->IsVisible()) {
					if(verticalcentering) {
						widget->Move(x,y+(maxh-widget->GetHeight())/2);
					}
					else {
						widget->Move(x,y);
					}

					if(
						(dynamic_cast<ISliderType*>(widget) && !dynamic_cast<ISliderType*>(widget)->IsVertical()) ||
						(dynamic_cast<textbox::Base*>(widget)) ||
						(dynamic_cast<ComboboxType*>(widget)) ||
						(dynamic_cast<Panel*>(widget)) ||
						(dynamic_cast<ListboxType*>(widget)) || 
						(dynamic_cast<FullWidth*>(widget)) || 
						(dynamic_cast<Label*>(widget) && col==(int)row->columns.size()-1 && maxsizedwidgets>0)
						) {

						widget->SetWidth(variablesize+carry);
						carry=0;

						x+=widget->GetWidth()+spacing;
						maxsizedwidgets--;
					}
					else if(colmaxw[col]>0 && dynamic_cast<Label*>(widget))
						x+=colmaxw[col]+spacing;
					else {
						if(widget->GetWidth()+x>attachedto->GetUsableWidth()-1 && attachedto->GetUsableWidth()-1-x>10)
							widget->SetWidth(attachedto->GetUsableWidth()-1-x);

						x+=widget->GetWidth()+spacing;
					}

					if(dynamic_cast<IButton*>(widget)==NULL)
						allbuttons=false;
				}

				col++;
			}

			if(allbuttons && row->columns.size()) {
				x-=spacing;
				if(buttonalign==TextAlignment::Left)
					x=0;
				else if(buttonalign==TextAlignment::Center)
					x=(attachedto->GetUsableWidth()-x)/2;
				else if(buttonalign==TextAlignment::Right)
					x=(attachedto->GetUsableWidth()-x);

				for(auto w=row->columns.begin();w!=row->columns.end();++w) {
					WidgetBase *widget=*w;
					if(widget->IsVisible()) {
						widget->SetX(x);
						x+=widget->GetWidth()+spacing;
					}
				}
			}

			if(row->height==-1) {
				y+=maxh+spacing;
			}
			else {
				y+=row->height;
			}
		}

		if(dynamic_cast<Window*>(attachedto) && autosizewindows && y>0) {
			Window *w=dynamic_cast<Window*>(attachedto);
			w->SetHeight(y+w->GetHeight()-w->GetUsableHeight()+1);
		}
		if(dynamic_cast<FullscreenPanel*>(attachedto) && autosizewindows && y>0) {
			FullscreenPanel *w=dynamic_cast<FullscreenPanel*>(attachedto);
			w->SetHeight(y+w->GetHeight()-w->GetUsableHeight()+1);
		}

		ReorganizeCompleted();
	}

	void LinearOrganizer::removefromrows(WidgetBase *w) {
		for(auto row=rows.begin();row!=rows.end();++row)
			for(auto col=row->columns.begin();col!=row->columns.end();++col)
				if((*col)==w) {
					for(;col!=row->columns.end()-1;++col)
						(*col)=*(col+1);
					row->columns.resize(row->columns.size()-1);
					return;
				}
	}

	void LinearOrganizer::sync() {
		if(!attachedto) return;

		for(auto w=organizedwidgets.First();w.IsValid();w.Next()) {
			if(w->GetContainer()!=attachedto) {
				removefromrows(w.CurrentPtr());
				organizedwidgets.Remove(*w);
				break;
			}
		}

		while(rows.size()>1) {
			if(rows.back().columns.size()==0 && rows[rows.size()-2].columns.size()==0) {
				rows.pop_back();
			}
			else
				break;
		}

		if(currentrow>=rows.size())
			currentrow=rows.size()-1;

		if(currentcol>=rows[currentrow].columns.size())
			currentcol=rows[currentrow].columns.size()-1;

		for(auto w=attachedto->Widgets.First();w.IsValid();w.Next()) {
			if(!organizedwidgets.Find(*w).IsValid()) {
				organizedwidgets.Add(w);
				rows[currentrow].columns.push_back(w.CurrentPtr());
			}
		}
	}

	LinearOrganizer::Modifier LinearOrganizer::endl=&LinearOrganizer::nextline_;
	LinearOrganizer::Modifier LinearOrganizer::nextline=&LinearOrganizer::nextline_;

}}
