#pragma once

#include "Checkbox/CheckboxBlueprint.h"
#include "Panel/PanelBlueprint.h"
#include "../Resource/Base.h"

namespace gge { namespace widgets {
	namespace tabpanel {
		class Blueprint;

		Blueprint *Load(resource::File& File, std::istream &Data, int Size);

		class Blueprint : public widgets::Blueprint, public resource::Base {
			friend Blueprint *Load(resource::File& File, std::istream &Data, int Size);
		public:

			Blueprint() : Placeholder(new widgets::Placeholder), Panel(NULL), Radio(NULL) { }

			widgets::Placeholder *Placeholder;
			checkbox::Blueprint *Radio;
			widgets::panel::Blueprint *Panel;

			void Prepare(GGEMain &main, resource::File &file);

			resource::GID::Type GetGID() const { return GID::Tabpanel; }

		protected:
			utils::SGuid panel, radio, placeholder;
		};
	}
}}
