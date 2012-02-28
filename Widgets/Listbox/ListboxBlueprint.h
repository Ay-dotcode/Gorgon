#pragma once


#pragma warning(push)
#pragma warning(disable:4351)


#include "..\Base\BluePrint.h"
#include "..\Definitions.h"
#include "..\..\Resource\Sound.h"

#include <map>
#include "..\Panel\PanelBlueprint.h"
#include "..\..\Resource\Base.h"
#include "..\Checkbox\CheckboxBlueprint.h"



namespace gge { namespace widgets {

	namespace listbox {

		class Blueprint : public widgets::Blueprint, public resource::Base {
			friend Blueprint *Load(resource::File& File, std::istream &Data, int Size);
		public:


			Blueprint() : Item(NULL), Header(NULL), AltItem(NULL), Footer(NULL), Panel(NULL)
			{
				DefaultSize=utils::Size(180,250);
			}



			virtual GID::Type getGID() const { return GID::Listbox; }


			virtual void Prepare(GGEMain &main, resource::File &file);

			checkbox::Blueprint *Item,
								*Header,
								*AltItem,
								*Footer;

			panel::Blueprint	*Panel;


		protected:
			utils::SGuid item;
			utils::SGuid header;
			utils::SGuid panel;
			utils::SGuid altitem;
			utils::SGuid footer;

		};

	}
} }


#pragma warning(pop)
