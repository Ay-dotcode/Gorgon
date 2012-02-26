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

		LinearOrganizer() : currentrow(), currentcol(), spacing(5), 
			INIT_PROPERTY(LinearOrganizer, Spacing) 
		{
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

			for(auto i=rg.First();i.isValid();i.Next()) {
				*attachedto<<*i;
				NextLine();
			}

			return *this;
		}

		LinearOrganizer &operator <<(const char *text) {
			Label *l=new Label(text);
			l->TextWrap=false;
			l->Autosize=AutosizeModes::Autosize;

			*this<<*l;

			return *this;
		}

		template <class T_>
		LinearOrganizer &operator <<(const std::string &text) {
			Label *l=new Label(text);

			*this<<l;
		}

		LinearOrganizer &operator <<(Modifier m) {
			m(*this);

			return *this;
		}

		utils::NumericProperty<LinearOrganizer, int> Spacing;



		static Modifier endl;
		static Modifier nextline;

	protected:
		utils::Collection<WidgetBase> organizedwidgets;
		std::vector<row> rows;
		int currentrow,currentcol;
		int spacing;

		static void nextline_(LinearOrganizer &organizer) {organizer.NextLine();}

		void setSpacing(const int &value) {
			if(Spacing!=value) {
				spacing = value;
				Reorganize();
			}
		}
		int getSpacing() const {
			return spacing;
		}

		void sync();
	};


}}
