#pragma once

#include "Base/Widget.h"
#include "Basic/Placeholder.h"
#include "Checkbox/CheckboxBlueprint.h"
#include "Panel/PanelBlueprint.h"
#include "../Resource/ResourceBase.h"

namespace gge { namespace widgets {
	
	namespace tabpanel {
		class Blueprint;

		Blueprint *Load(resource::File& File, std::istream &Data, int Size);

		class Blueprint : public widgets::Blueprint, public resource::ResourceBase {
			friend Blueprint *Load(resource::File& File, std::istream &Data, int Size);
		public:

			widgets::Placeholder Placeholder;
			checkbox::Blueprint *Radio;
			widgets::panel::Blueprint *Panel;

			void Prepare(GGEMain &main, resource::File &file);

			resource::GID::Type getGID() const { return GID::Tabpanel; }

		protected:
			utils::SGuid panel, radio, placeholder;
		};
	}

	class Tabpanel : public WidgetBase {
	public:
		
	};

}}
