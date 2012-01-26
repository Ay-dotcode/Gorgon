#include "TabPanel.h"
#include "../Resource/ResourceFile.h"

using namespace gge::resource;

namespace gge { namespace widgets {

	namespace tabpanel {


		Blueprint * Load(resource::File& File, std::istream &Data, int Size) {

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
				else if(gid==GID::Tabpanel_Props) {

					ReadFrom(Data, bp->DefaultSize);
					bp->panel.Load(Data);
					bp->radio.Load(Data);
					bp->placeholder.Load(Data);


					EatChunk(Data, size-(1 * 8 + 3 * 8));
				}
				else {
					ResourceBase *res=File.LoadObject(Data, gid,size);
					if(res)
						bp->Subitems.Add(res, bp->Subitems.HighestOrder()+1);
				}
			}


			return bp;
		}

		void Blueprint::Prepare(GGEMain &main, resource::File &file) {
			ResourceBase::Prepare(main, file);

			file.FindObject(panel, Panel);
			file.FindObject(radio, Radio);
			widgets::Placeholder *p;
			file.FindObject(placeholder, p);
			if(p)
				Placeholder=*p;
		}

	}

}}
