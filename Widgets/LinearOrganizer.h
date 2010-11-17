#pragma once

#include "Frame.h"
#include "Spinner.h"
#include "RadioGroup.h"
#include "WidgetRegistry.h"
#include "../Utils/Size2D.h"

namespace gorgonwidgets {

	class LinearOrganizerLine;
	class LinearOrganizerColumn;
	class LinearOrganizer;

	enum LinearOrganizerSizeMode {
		autosize,
		fixedsize
	};

	class LinearOrganizerObject {
	public:
		LinearOrganizerObject(IWidgetObject &object, LinearOrganizerLine *parent);

		IWidgetObject &object;
		
		LinearOrganizerSizeMode Sizemode;
		Bounds Padding;
		Alignment Align;
		Size2D Minimum;
		Size2D Maximum;
		Size2D Size;

		Size2D ResizeTo(Bounds Cell);

		LinearOrganizerLine *parent;
	};

	class LinearOrganizerLine {
	public:
		LinearOrganizerLine(LinearOrganizerColumn *parent) :
			parent(parent), LineHeight(0) { }

		LinearOrganizerColumn *parent;

		Collection<LinearOrganizerObject> Objects;

		int LineHeight;

		~LinearOrganizerLine() { Objects.Destroy(); }
	};
	
	class LinearOrganizerColumn {
	public:
		LinearOrganizerColumn(LinearOrganizer *parent) : parent(parent) {
			Lines.Add(new LinearOrganizerLine(this));
		}
		LinearOrganizerLine *AddLine() {
			Lines.Add(new LinearOrganizerLine(this));
			return Lines[Lines.getCount()-1];
		}

		LinearOrganizer *parent;
		Collection<LinearOrganizerLine> Lines;

		~LinearOrganizerColumn() { Lines.Destroy(); }
		
	};

	typedef void (*LinearOrganizerModifier)(LinearOrganizer &);

	class LinearOrganizer : public Frame {
		friend void lo_nextline_(LinearOrganizer &organizer);
		friend void lo_fixedmode_(LinearOrganizer &organizer);
	public:
		LinearOrganizer(FrameBP &BluePrint, int X=0,int Y=0, int W=100, int H=100) : 
			Frame(BluePrint, X,Y, W,H)
		{
			init();
		}

		LinearOrganizer(FrameBP &BluePrint, IWidgetContainer &container, int X=0,int Y=0, int W=100, int H=100) : 
			Frame(BluePrint, X,Y, W,H)
		{
			init();
			Frame::SetContainer(&container);
		}

		virtual IWidgetObject *AddWidget(IWidgetObject *Widget);
		virtual IWidgetObject *RemoveWidget(IWidgetObject *Widget);
		virtual IWidgetObject &AddWidget(IWidgetObject &Widget) { return *AddWidget(&Widget); }
		virtual IWidgetObject &RemoveWidget(IWidgetObject &Widget) { return *RemoveWidget(&Widget); }

		void setPadding(int Padding) { setPadding(Bounds(Padding,Padding,Padding,Padding)); }
		void setPadding(Bounds Padding) {
			this->Padding=Padding;
			adjustlocations();
		}
		template<class T_>
		LinearOrganizer &operator <<(T_ &object) {
			if(object.Height()>currentline->LineHeight)
				currentline->LineHeight=object.Height();

			currentline->Objects.Add(new LinearOrganizerObject(object, currentline));

			Frame::AddWidget(object);

			return *this;
		}

		template<>
		LinearOrganizer &operator <<(Spinner &object) {
			if(object.isAttached())
				Frame::AddWidget(object.getHost());

			*this<<(IWidgetObject&)object;
	
			return *this;
		}

		template<class O_>
		LinearOrganizer &operator <<(RadioButtonGroup<O_> &object) {
			foreach(IRadioButton, radio, object.Buttons) {
				*this<<radio->getWidget()<<LinearOrganizer::nextline;
			}

			return *this;
		}

		LinearOrganizer &operator <<(string text) {
			if(text.substr(0,2)=="**" && text.substr(text.length()-2,2)=="**")
				*this<<WidgetRegistry.createBoldLabel(text.substr(2,text.length()-4));
			else
				*this<<WidgetRegistry.createFieldLabel(text);

			return *this;
		}

		LinearOrganizer &operator <<(const char *text) {
			*this<<string(text);

			return *this;
		}

		template<>
		LinearOrganizer &operator <<(LinearOrganizerModifier &modifier) {
			modifier(*this);

			return *this;
		}

		LinearOrganizer &operator >>(IWidgetObject &object) { 
			RemoveWidget(object); 
			return *this; 
		}

		void RemoveAll() {
			Columns.Destroy();


			currentcolumnnum=0;
			currentlinenum=0;

			Columns.Add(new LinearOrganizerColumn(this));
			currentcolumn=Columns[0];
			currentline=currentcolumn->Lines[0];

			foreach(IWidgetObject, object, Subobjects)
				object->Detach();
		}


		static LinearOrganizerModifier nextline;
		static LinearOrganizerModifier fixedmode;
		static LinearOrganizerModifier removeall;

	protected:
		Bounds Padding;
		int IntegralHeight;
		int ColumnDistance;


		int currentlinenum;
		int currentcolumnnum;
		LinearOrganizerSizeMode currentsizemode;

		LinkedList<LinearOrganizerColumn> Columns;
		LinearOrganizerColumn *currentcolumn;
		LinearOrganizerLine *currentline;

		virtual void adjustlocations();
		void init();
	};

	inline void lo_nextline_(LinearOrganizer &organizer) {
		organizer.currentlinenum++;
		if(organizer.currentcolumn->Lines.getCount()<=organizer.currentlinenum)
			organizer.currentline = organizer.currentcolumn->AddLine();
	}

	inline void lo_fixedmode_(LinearOrganizer &organizer) { organizer.currentsizemode=fixedsize; }
	inline void lo_removeall_(LinearOrganizer &organizer) { organizer.RemoveAll(); }

}
