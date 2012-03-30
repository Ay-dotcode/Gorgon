#pragma once


#include "../Base/Organizer.h"
#include "../Base/Container.h"
#include "../../Utils/Property.h"
#include "../RadioButton.h"
#include "../Label.h"

namespace gge { namespace widgets {


	//If you are using linear organizer, you should add widgets to
	//Linear organizer, not to container. If you add widget to container
	//it will be placed at the end. (???) However, you can remove widgets from 
	//container
	class LinearOrganizer : public Organizer {
	protected:


		class row {
		public:
			row() : height(-1) {
				
			}

			std::vector<WidgetBase*> columns;

			int height; //-1 = auto
		};

	public:

		typedef void(*Modifier)(LinearOrganizer &organizer);

		LinearOrganizer() : currentrow(), currentcol(), spacing(5), autosizewindows(true),
			buttonalign(TextAlignment::Right),verticalcentering(false),
			INIT_PROPERTY(LinearOrganizer, Spacing),
			INIT_PROPERTY(LinearOrganizer, AutosizeWindows),
			INIT_PROPERTY(LinearOrganizer, ButtonAlign),
			INIT_PROPERTY(LinearOrganizer, VerticalCentering)
		{
			spacing=WR.WidgetSpacing.x;
			rows.push_back(row());
		}
		
		virtual void Reorganize();


		virtual ~LinearOrganizer() { }

		void NextLine(int height) {
			currentrow++;
			currentcol=0;
			if((unsigned)currentrow<rows.size()) {
				rows[currentrow].height=height;
			}
			else {
				row r;
				r.height=height;
				rows.push_back(r);
			}
		}

		void NextLine() {
			NextLine(-1);
		}

		template<class T_>
		LinearOrganizer &operator <<(T_ &w) {
			if(!attachedto) return *this;

			*attachedto<<w;

			return *this;
		}

		template <class T_>
		LinearOrganizer &operator <<(RadioGroup<T_> &rg) {
			if(!attachedto) return *this;

			for(auto i=rg.First();i.IsValid();i.Next()) {
				*attachedto<<*i;
				NextLine();
			}

			return *this;
		}

		LinearOrganizer &operator <<(const char *text) {
			return operator <<(std::string(text));
		}

		LinearOrganizer &operator <<(const std::string &text) {
			std::string txt=text;

			if(txt=="") return *this;

			Label *l=new Label;

			if(txt[0]=='*' && txt[txt.length()-1]=='*') {
				txt=txt.substr(1, txt.length()-2);
				l->SetBlueprint(WR.Labels.Bold);
			}

			l->Text=txt;
			l->TextWrap=false;
			l->Autosize=AutosizeModes::Autosize;

			*this<<*l;

			return *this;
		}

		LinearOrganizer &operator <<(Modifier m) {
			m(*this);

			return *this;
		}

		utils::NumericProperty<LinearOrganizer, int> Spacing;
		utils::BooleanProperty<LinearOrganizer> AutosizeWindows;
		utils::Property<LinearOrganizer, gge::TextAlignment::Type> ButtonAlign;
		utils::BooleanProperty<LinearOrganizer> VerticalCentering;



		static Modifier endl;
		static Modifier nextline;

	protected:
		utils::Collection<WidgetBase> organizedwidgets;
		std::vector<row> rows;
		int currentrow,currentcol;
		int spacing;
		bool autosizewindows;
		TextAlignment::Type buttonalign;
		bool verticalcentering;

		static void nextline_(LinearOrganizer &organizer) {organizer.NextLine();}

		void setSpacing(const int &value) {
			if(spacing!=value) {
				spacing = value;
				Reorganize();
			}
		}
		int getSpacing() const {
			return spacing;
		}

		void setVerticalCentering(const bool &value) {
			if(verticalcentering!=value) {
				verticalcentering = value;
				Reorganize();
			}
		}
		bool getVerticalCentering() const {
			return verticalcentering;
		}

		void setButtonAlign(const TextAlignment::Type &value) {
			if(buttonalign!=value) {
				buttonalign = value;
				Reorganize();
			}
		}
		TextAlignment::Type getButtonAlign() const {
			return ButtonAlign;
		}

		void setAutosizeWindows(const bool &value) {
			if(autosizewindows!=value) {
				autosizewindows = value;
				
				Reorganize();
			}
		}
		bool getAutosizeWindows() const {
			return autosizewindows;
		}

		void sync();
	};


}}
