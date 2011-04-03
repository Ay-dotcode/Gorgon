#include "LinearOrganizer.h"

namespace gge { namespace widgets {

	IWidgetObject *LinearOrganizer::AddWidget(IWidgetObject *Widget) {
		currentline->Objects.Add(new LinearOrganizerObject(*Widget, currentline));

		return Frame::AddWidget(Widget);
	}

	IWidgetObject *LinearOrganizer::RemoveWidget(IWidgetObject *Widget) {
		foreach(LinearOrganizerColumn, column, Columns) {
			foreach(LinearOrganizerLine, line, column->Lines) {
				foreach(LinearOrganizerObject, object, line->Objects) {
					if(&object->object==Widget) {
						line->Objects.Remove(object);
						goto out;
					}
				}
			}
		}

out:
		adjustlocations();
		return Frame::RemoveWidget(Widget);
	}

	void LinearOrganizer::adjustlocations() {
		static bool guard=false;
		if(!guard)
			guard=true;
		else
			return;

		int colx=0;
		foreach(LinearOrganizerColumn, column, Columns) {
			int y=0;
			foreach(LinearOrganizerLine, line, column->Lines) {
				y+=Padding.Top;

				int maxy=0;
				foreach(LinearOrganizerObject, object, line->Objects) {
					if(maxy<object->object.Height())
						maxy=object->object.Height();
				}
				line->LineHeight=maxy;


				int x=colx;
				foreach(LinearOrganizerObject, object, line->Objects) {
					x+=Padding.Left;
					object->object.Move(x, y + (line->LineHeight-object->object.Height())/2 );
					x+=object->object.Width()+Padding.Right;
				}
				line->LineWidth=x;

				y+=line->LineHeight+Padding.Bottom;
				line->LineHeight+=Padding.Top+Padding.Bottom;
			}
		}
				

		Frame::adjustlocations();
		guard=false;
	}

	void LinearOrganizer::init() {
		currentcolumnnum=0;
		currentlinenum=0;

		Columns.Add(new LinearOrganizerColumn(this));
		currentcolumn=Columns[0];
		currentline=currentcolumn->Lines[0];

		Padding=Margins(2,2, 2,2);
		IntegralHeight=0;
		ColumnDistance=15; 
		currentsizemode=autosize;
	}

	LinearOrganizerObject::LinearOrganizerObject(IWidgetObject &object, LinearOrganizerLine *parent) :
		object(object), parent(parent)
	{ }

	LinearOrganizerModifier LinearOrganizer::nextline=&lo_nextline_;
	LinearOrganizerModifier LinearOrganizer::fixedmode=&lo_fixedmode_;
	LinearOrganizerModifier LinearOrganizer::removeall=&lo_removeall_;
} }

