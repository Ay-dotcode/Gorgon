#include "PanelBlueprint.h"
#include "../../Engine/Pointer.h"
#include "../../Resource/File.h"

using namespace gge::resource;
using namespace std;
using namespace gge::animation;
using namespace gge::graphics;

namespace gge { namespace widgets {

	//only service classes will be here
	namespace panel {

		Blueprint::Element *LoadElement(resource::File& File, std::istream &Data, int Size) {
			Blueprint::Element *bp=new Blueprint::Element;

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
				else if(gid==GID::Panel_Element_Props) {
					int ptarget=int(Data.tellg());

					ReadFrom(Data, bp->Duration);
					bp->innerborder.Load(Data);
					bp->outerborder.Load(Data);
					bp->scrollingborder.Load(Data);
					bp->resizer.Load(Data);
					bp->resizerplace.Load(Data);
					bp->Opacity=255-ReadFrom<int>(Data);
					bp->BGOpacity=255-ReadFrom<int>(Data);
					bp->titleborder.Load(Data);
					bp->dialogbuttonborder.Load(Data);
					bp->sound.Load(Data);
					bp->overlay.Load(Data);

					Data.seekg(ptarget+size);
				}
				else {
					resource::Base *res=File.LoadObject(Data, gid,size);
					if(res)
						bp->Subitems.Add(res, bp->Subitems.HighestOrder()+1);
				}
			}

			return bp;
		}

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
				else if(gid==GID::Panel_Props) {
					bp->Pointer=(Pointer::PointerType	)ReadFrom<int>(Data);
					ReadFrom(Data, bp->DefaultSize);
					bp->scroller.Load(Data);
					bp->titlelabel.Load(Data);
					bp->titlebutton.Load(Data);
					bp->titlebuttonplace.Load(Data);
					bp->dialogbutton.Load(Data);
					bp->dialogbuttonplace.Load(Data);

					ReadFrom(Data, bp->AlphaAnimation);

					bp->normal.Load(Data);
					bp->active.Load(Data);
					bp->moving.Load(Data);
					bp->disabled.Load(Data);

					bp->normaltoactive.Load(Data);
					bp->activetomoving.Load(Data);
					bp->normaltodisabled.Load(Data);

					bp->activetonormal.Load(Data);
					bp->movingtoactive.Load(Data);
					bp->disabledtonormal.Load(Data);


					EatChunk(Data, size-(4 * 2 + 8 * 1 + 16 * 16));
				}
				else if(gid==GID::Panel_Element) {
					Blueprint::Element *elm=LoadElement(File, Data, size);

					bp->Subitems.Add(elm, bp->Subitems.HighestOrder()+1);
				}
				else {
					resource::Base *res=File.LoadObject(Data, gid,size);
					if(res)
						bp->Subitems.Add(res, bp->Subitems.HighestOrder()+1);
				}
			}


			return bp;
		}



		void Blueprint::Element::Prepare(GGEMain &main, resource::File &file) {
			resource::Base::Prepare(main,file);

			file.FindObject(innerborder, InnerBorder);
			file.FindObject(outerborder, OuterBorder);
			file.FindObject(scrollingborder, ScrollingBorder);
			file.FindObject(resizer, Resizer);
			file.FindObject(resizerplace, ResizerPlace);
			file.FindObject(titleborder, TitleBorder);
			file.FindObject(sound, Sound);
			file.FindObject(overlay, Overlay);
		}


		void Blueprint::Prepare(GGEMain &main, resource::File &file) {
			Base::Prepare(main, file);

			file.FindObject(scroller, Scroller);
			file.FindObject(titlelabel, TitleLabel);
			file.FindObject(titlebutton, TitleButton);
			file.FindObject(titlebuttonplace, TitleButtonPlace);
			file.FindObject(dialogbutton, DialogButton);
			file.FindObject(dialogbuttonplace, DialogButtonPlace);


			file.FindObject(normal, Normal);

			file.FindObject(active, Active);

			file.FindObject(moving, Moving);

			file.FindObject(disabled, Disabled);


			file.FindObject(normaltoactive, NormalToActive);

			file.FindObject(normaltodisabled, NormalToDisabled);

			file.FindObject(activetomoving, ActiveToMoving);


			file.FindObject(activetonormal, ActiveToNormal);

			file.FindObject(disabledtonormal, DisabledToNormal);

			file.FindObject(movingtoactive, MovingToActive);


			updatemapping();

		}

		void Blueprint::updatemapping() {
			Mapping[widgets::Blueprint::Normal][Blueprint::Style_None]=Normal;
			Mapping[widgets::Blueprint::Hover][Blueprint::Style_None]=Active;
			Mapping[widgets::Blueprint::Down][Blueprint::Style_None]=Moving;
			Mapping[widgets::Blueprint::Disabled][Blueprint::Style_None]=Disabled;
			Mapping[widgets::Blueprint::Normal][widgets::Blueprint::Hover]=NormalToActive;
			Mapping[widgets::Blueprint::Normal][widgets::Blueprint::Disabled]=NormalToDisabled;
			Mapping[widgets::Blueprint::Hover][widgets::Blueprint::Down]=ActiveToMoving;
			Mapping[widgets::Blueprint::Hover][widgets::Blueprint::Normal]=ActiveToNormal;
			Mapping[widgets::Blueprint::Disabled][widgets::Blueprint::Normal]=DisabledToNormal;
			Mapping[widgets::Blueprint::Down][widgets::Blueprint::Hover]=MovingToActive;
		}

	}

}}
