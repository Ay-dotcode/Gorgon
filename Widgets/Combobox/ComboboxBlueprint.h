#pragma once


#include "../Textbox/TextboxBlueprint.h"
#include "../Listbox/ListboxBlueprint.h"
#include "../Checkbox/CheckboxBlueprint.h"

namespace gge { namespace widgets {
	namespace combobox {
		//!Requires ensure visible

		class Blueprint;

		Blueprint *Load(resource::File& File, std::istream &Data, int Size);

		class Blueprint : public resource::Base, public widgets::Blueprint {
			friend Blueprint *Load(resource::File& File, std::istream &Data, int Size);
		public:

			Blueprint() : Textbox(NULL), 
				Listbox(NULL), Dropbutton(NULL), ListMargins(0,0)
			{ }

			textbox::Blueprint *Textbox;
			listbox::Blueprint *Listbox;
			checkbox::Blueprint *Dropbutton;

			utils::Margins ListMargins;


			virtual GID::Type getGID() const { return GID::Combobox; }


			virtual void Prepare(GGEMain &main, resource::File &file);


		protected:
			utils::SGuid textbox,listbox,dropbutton;
		};
	}
}}
