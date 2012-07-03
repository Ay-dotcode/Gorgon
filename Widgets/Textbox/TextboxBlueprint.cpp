#include "TextboxBlueprint.h"
#include "../../Engine/Pointer.h"
#include "../../Resource/File.h"

using namespace gge::resource;
using namespace std;
using namespace gge::animation;
using namespace gge::graphics;


namespace gge { namespace widgets {

	namespace textbox {


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
				else if(gid==GID::Textbox_Element_Props) {
					int ptarget=int(Data.tellg());
					int sz;

					ReadFrom(Data, bp->Duration);

					ReadFrom<int>(Data);//gid
					sz=ReadFrom<int>(Data);//size
					bp->font=Font::Load(File, Data, sz);

					bp->innerborder.Load(Data);
					bp->outerborder.Load(Data);
					bp->sound.Load(Data);

					ReadFrom<int>(Data);
					sz=ReadFrom<int>(Data);
					bp->selectedfont=Font::Load(File, Data, sz);

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
				else if(gid==GID::Textbox_Props) {

					bp->typesound.Load(Data);
					bp->Pointer=(Pointer::PointerType	)ReadFrom<int>(Data);
					bp->selection.Load(Data);
					bp->caretplace.Load(Data);
					bp->caret.Load(Data);
					ReadFrom(Data, bp->DefaultSize);
					bp->Align=(Alignment::Type)ReadFrom<int>(Data);
					bp->scroller.Load(Data);
					ReadFrom(Data, bp->AlphaAnimation);

					bp->normal.Load(Data);
					bp->hover.Load(Data);
					bp->focused.Load(Data);
					bp->disabled.Load(Data);

					bp->normaltohover.Load(Data);
					bp->normaltofocused.Load(Data);
					bp->hovertofocused.Load(Data);
					bp->normaltodisabled.Load(Data);

					bp->hovertonormal.Load(Data);
					bp->focusedtonormal.Load(Data);
					bp->focusedtohover.Load(Data);
					bp->disabledtonormal.Load(Data);



					EatChunk(Data, size-(4 * 3 + 8 * 1 + 8 * 17));
				}
				else if(gid==GID::Textbox_Element) {
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
			Font=new gge::Font((gge::Font)font);
			if(!Font->Theme)
				utils::CheckAndDelete(Font);

			SelectedFont=new gge::Font((gge::Font)selectedfont);
			if(!SelectedFont->Theme)
				utils::CheckAndDelete(SelectedFont);

			file.FindObject(sound, Sound);
			file.FindObject(overlay, Overlay);
		}


		void Blueprint::Prepare(GGEMain &main, resource::File &file) {
			Base::Prepare(main, file);

			file.FindObject(scroller, Scroller);
			file.FindObject(selection, Selection);
			file.FindObject(typesound, TypeSound);
			Placeholder *cp;
			file.FindObject(caretplace, cp);
			if(cp) {
				utils::CheckAndDelete(CaretPlace);
				CaretPlace=cp;
			}
			file.FindObject(caret, Caret);


			file.FindObject(normal, Normal);

			file.FindObject(hover, Hover);

			file.FindObject(focused, Focused);

			file.FindObject(disabled, Disabled);


			file.FindObject(normaltohover, NormalToHover);

			file.FindObject(normaltofocused, NormalToFocused);

			file.FindObject(normaltodisabled, NormalToDisabled);

			file.FindObject(hovertofocused, HoverToFocused);


			file.FindObject(hovertonormal, HoverToNormal);

			file.FindObject(focusedtonormal, FocusedToNormal);

			file.FindObject(disabledtonormal, DisabledToNormal);

			file.FindObject(focusedtohover, FocusedToHover);

			updatemapping();

		}

		void Blueprint::updatemapping() {
			Mapping[widgets::Blueprint::Normal][Blueprint::Style_None]=Normal;
			Mapping[widgets::Blueprint::Hover][Blueprint::Style_None]=Hover;
			Mapping[widgets::Blueprint::Focused_Style][Blueprint::Style_None]=Focused;
			Mapping[widgets::Blueprint::Disabled][Blueprint::Style_None]=Disabled;

			Mapping[widgets::Blueprint::Normal][widgets::Blueprint::Hover]=NormalToHover;
			Mapping[widgets::Blueprint::Normal][widgets::Blueprint::Focused_Style]=NormalToFocused;
			Mapping[widgets::Blueprint::Normal][widgets::Blueprint::Disabled]=NormalToDisabled;
			Mapping[widgets::Blueprint::Hover][widgets::Blueprint::Focused_Style]=HoverToFocused;

			Mapping[widgets::Blueprint::Hover][widgets::Blueprint::Normal]=HoverToNormal;
			Mapping[widgets::Blueprint::Focused_Style][widgets::Blueprint::Normal]=FocusedToNormal;
			Mapping[widgets::Blueprint::Disabled][widgets::Blueprint::Normal]=DisabledToNormal;
			Mapping[widgets::Blueprint::Focused_Style][widgets::Blueprint::Hover]=FocusedToHover;
		}


	}

}}
