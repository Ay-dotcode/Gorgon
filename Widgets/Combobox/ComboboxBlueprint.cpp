#include "ComboboxBlueprint.h"
#include "../../Resource/File.h"

using namespace gge::resource;
using namespace gge::utils;


namespace gge { namespace widgets {
	namespace combobox {


		Blueprint *Load(resource::File& File, std::istream &Data, int Size) {
			//Load everything here

			Blueprint *bp=new Blueprint;

			int target=Data.tellg()+Size;
			while(Data.tellg()<target) {
				int gid,size;
				ReadFrom(Data, gid);
				ReadFrom(Data, size);

				if(gid==resource::GID::Guid) {
					bp->guid.LoadLong(Data);
				}
				else if(gid==resource::GID::SGuid) {
					bp->guid.Load(Data);
				}
				else if(gid==GID::Combobox_Props) {
					ReadFrom(Data, bp->DefaultSize);
					bp->textbox.Load(Data);
					bp->listbox.Load(Data);
					bp->dropbutton.Load(Data);
					ReadFrom(Data, bp->ListMargins);

					EatChunk(Data, size-(1 * 8 + 3 * 8 + 1*16));
				}
				else {
					resource::Base *res=File.LoadObject(Data, gid,size);
					if(res)
						bp->Subitems.Add(res, bp->Subitems.HighestOrder()+1);
				}
			}


			return bp;
		}



		void Blueprint::Prepare(GGEMain &main, resource::File &file) {
			Base::Prepare(main, file);

			file.FindObject(textbox, Textbox);
			file.FindObject(listbox, Listbox);
			file.FindObject(dropbutton, Dropbutton);
		}

	}
}}