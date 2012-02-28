#include "ListboxBlueprint.h"
#include "..\..\Engine\Pointer.h"
#include "..\..\Resource\File.h"

using namespace gge::resource;
using namespace std;
using namespace gge::animation;
using namespace gge::graphics;


namespace gge { namespace widgets {

	namespace listbox {


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
				else if(gid==GID::Listbox_Props) {

					ReadFrom(Data, bp->DefaultSize);
					bp->item.Load(Data);
					bp->header.Load(Data);
					bp->panel.Load(Data);
					bp->altitem.Load(Data);
					bp->footer.Load(Data);


					EatChunk(Data, size-(1 * 8 + 5 * 8));
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

			file.FindObject(item, Item);
			file.FindObject(header, Header);
			file.FindObject(panel, Panel);
			file.FindObject(altitem, AltItem);
			file.FindObject(footer, Footer);
		}


	}

}}
