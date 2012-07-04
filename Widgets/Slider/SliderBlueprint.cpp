#include "SliderBlueprint.h"
#include "../../Engine/Pointer.h"
#include "../../Resource/File.h"

using namespace gge::resource;
using namespace std;
using namespace gge::animation;
using namespace gge::graphics;

namespace gge { namespace widgets {

	//only service classes will be here
	namespace slider {

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
				else if(gid==GID::Slider_Element_Props) {
					int ptarget=int(Data.tellg());

					ReadFrom<int>(Data);
					int sz=ReadFrom<int>(Data);
					bp->font=Font::Load(File, Data, sz);
					ReadFrom<int>(Data);
					sz=ReadFrom<int>(Data);
					bp->valuefont=Font::Load(File, Data, sz);

					ReadFrom(Data, bp->Duration);
					bp->sound.Load(Data);

					bp->border.Load(Data);
					bp->overlay.Load(Data);

					bp->symbol.Load(Data);
					bp->symbolplace.Load(Data);
					bp->textplace.Load(Data);
					bp->valueplace.Load(Data);

					bp->ruler.Load(Data);
					bp->indicator.Load(Data);
					bp->ruleroverlay.Load(Data);

					bp->tickmark.Load(Data);
					bp->tickmarkplace.Load(Data);
					bp->tickmarkborder.Load(Data);

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

		Blueprint::Group *LoadGroup(resource::File& File, std::istream &Data, int Size) {
			Blueprint::Group *bp=new Blueprint::Group;

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
				else if(gid==GID::Slider_Group_Props) {
					bp->Orientation = (Blueprint::OrientationType)ReadFrom<int>(Data);
					bp->Focus = Blueprint::FocusType(ReadFrom<int>(Data));
					
					bp->upbutton.Load(Data);
					bp->downbutton.Load(Data);
					bp->upplace.Load(Data);
					bp->downplace.Load(Data);

					bp->normal.Load(Data);
					bp->hover.Load(Data);
					bp->down.Load(Data);
					bp->disabled.Load(Data);

					bp->normaltohover.Load(Data);
					bp->normaltodown.Load(Data);
					bp->normaltodisabled.Load(Data);
					bp->hovertodown.Load(Data);

					bp->hovertonormal.Load(Data);
					bp->downtonormal.Load(Data);
					bp->disabledtonormal.Load(Data);
					bp->downtohover.Load(Data);

					EatChunk(Data, size-(4 * 2 + 8 * 16));
				}
				else if(gid==GID::Slider_Element) {
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
				else if(gid==GID::Slider_Props) {
					bp->Pointer=(Pointer::PointerType	)ReadFrom<int>(Data);
					ReadFrom(Data, bp->DefaultSize);
					bp->ticksound.Load(Data);
					ReadFrom(Data, bp->AlphaAnimation);

					EatChunk(Data, size-(4 * 2 + 8 * 1 + 8 * 1));
				}
				else if(gid==GID::Slider_Group) {
					Blueprint::Group *grp=LoadGroup(File, Data, size);

					bp->Subitems.Add(grp, bp->Subitems.HighestOrder()+1);
				}
				else {
					resource::Base *res=File.LoadObject(Data, gid,size);
					if(res)
						bp->Subitems.Add(res, bp->Subitems.HighestOrder()+1);
				}
			}

			for(auto it=bp->Subitems.First();it.IsValid();it.Next()) {
				Blueprint::Group &g=dynamic_cast<Blueprint::Group &>(*it);

				bp->Mapping[g] = &g;
			}


			return bp;
		}



		void Blueprint::Element::Prepare(GGEMain &main, resource::File &file) {
			resource::Base::Prepare(main,file);

			Border				=dynamic_cast<BorderDataResource*>(file.FindObject(border));
			Symbol				=dynamic_cast<animation::RectangularGraphic2DSequenceProvider*>(file.FindObject(symbol));
			Font				=new gge::Font((gge::Font)font);
			if(!Font->Theme)
				utils::CheckAndDelete(Font);
			ValueFont				=new gge::Font((gge::Font)valuefont);
			if(!ValueFont->Theme)
				utils::CheckAndDelete(ValueFont);
			SymbolPlace			=dynamic_cast<Placeholder*>(file.FindObject(symbolplace));
			TextPlace			=dynamic_cast<Placeholder*>(file.FindObject(textplace));
			ValuePlace			=dynamic_cast<Placeholder*>(file.FindObject(valueplace));
			Sound				=dynamic_cast<resource::Sound*>(file.FindObject(sound));
			Overlay				=dynamic_cast<BorderDataResource*>(file.FindObject(overlay));

			Ruler				=dynamic_cast<BorderDataResource*>(file.FindObject(ruler));
			Indicator			=dynamic_cast<BorderDataResource*>(file.FindObject(indicator));
			RulerOverlay		=dynamic_cast<BorderDataResource*>(file.FindObject(ruleroverlay));

			Tickmark			=dynamic_cast<RectangularGraphic2DSequenceProvider*>(file.FindObject(tickmark));
			TickmarkPlace		=dynamic_cast<Placeholder*>(file.FindObject(tickmarkplace));
			TickmarkBorder		=dynamic_cast<BorderDataResource*>(file.FindObject(tickmarkborder));
		}

		void Blueprint::Group::Prepare(GGEMain &main, resource::File &file) {
			resource::Base::Prepare(main,file);

			UpButton		=dynamic_cast<checkbox::Blueprint*>(file.FindObject(upbutton));
			DownButton		=dynamic_cast<checkbox::Blueprint*>(file.FindObject(downbutton));
			UpPlace			=dynamic_cast<Placeholder*>(file.FindObject(upplace));
			DownPlace		=dynamic_cast<Placeholder*>(file.FindObject(downplace));


			Normal			=dynamic_cast<Blueprint::Element*>(file.FindObject(normal));

			Hover			=dynamic_cast<Blueprint::Element*>(file.FindObject(hover));

			Down			=dynamic_cast<Blueprint::Element*>(file.FindObject(down));

			Disabled		=dynamic_cast<Blueprint::Element*>(file.FindObject(disabled));


			NormalToHover	=dynamic_cast<Blueprint::Element*>(file.FindObject(normaltohover));

			NormalToDown	=dynamic_cast<Blueprint::Element*>(file.FindObject(normaltodown));

			NormalToDisabled=dynamic_cast<Blueprint::Element*>(file.FindObject(normaltodisabled));

			HoverToDown		=dynamic_cast<Blueprint::Element*>(file.FindObject(hovertodown));


			HoverToNormal	=dynamic_cast<Blueprint::Element*>(file.FindObject(hovertonormal));

			DownToNormal	=dynamic_cast<Blueprint::Element*>(file.FindObject(downtonormal));

			DisabledToNormal=dynamic_cast<Blueprint::Element*>(file.FindObject(disabledtonormal));

			DownToHover		=dynamic_cast<Blueprint::Element*>(file.FindObject(downtohover));


			updatemapping();
		}

		void Blueprint::Group::updatemapping() {
			Mapping[Blueprint::Normal][Blueprint::Style_None]=Normal;
			Mapping[Blueprint::Hover][Blueprint::Style_None]=Hover;
			Mapping[Blueprint::Down][Blueprint::Style_None]=Down;
			Mapping[Blueprint::Disabled][Blueprint::Style_None]=Disabled;
			Mapping[Blueprint::Normal][Blueprint::Hover]=NormalToHover;
			Mapping[Blueprint::Normal][Blueprint::Down]=NormalToDown;
			Mapping[Blueprint::Normal][Blueprint::Disabled]=NormalToDisabled;
			Mapping[Blueprint::Hover][Blueprint::Down]=HoverToDown;
			Mapping[Blueprint::Hover][Blueprint::Normal]=HoverToNormal;
			Mapping[Blueprint::Down][Blueprint::Normal]=DownToNormal;
			Mapping[Blueprint::Disabled][Blueprint::Normal]=DisabledToNormal;
			Mapping[Blueprint::Down][Blueprint::Hover]=DownToHover;
		}

		Blueprint::GroupMode::GroupMode(const Group &g) : focus(g.Focus), orientation(g.Orientation) {

		}


		void Blueprint::Prepare(GGEMain &main, resource::File &file) {
			Base::Prepare(main, file);

			TickSound			=dynamic_cast<resource::Sound*>(file.FindObject(ticksound));
		}

		Blueprint::AnimationInfo Blueprint::HasStyleAnimation(OrientationType o, FocusMode focus, StyleMode style) const {
			Blueprint::AnimationInfo ret;

			ret=hasstyleanimation(o, focus, style);
			if(ret)
				return ret;

			if(focus.to!=Blueprint::Focus_None) {
				ret=hasstyleanimation(o, focus.swap(), style);
				if(ret)
					return ret;
			}

			ret=hasstyleanimation(o, FocusMode(), style);
			if(ret)
				return ret;

			return Missing;
		}

		Blueprint::AnimationInfo Blueprint::HasFocusAnimation(OrientationType o, FocusMode focus) const {
			if(Mapping[GroupMode(o, focus)])
				return Blueprint::AnimationInfo(Blueprint::Forward,Mapping[GroupMode(o, focus)]->Normal->Duration);

			if(Mapping[GroupMode(o, focus.swap())])
				return Blueprint::AnimationInfo(Blueprint::Backward,Mapping[GroupMode(o, focus.swap())]->Normal->Duration);

			return Blueprint::Missing;
		}

		void Blueprint::GetAlternatives(Group** &groups, OrientationType o, FocusMode focus) const {
			typedef Blueprint::GroupMode gm;
			typedef Blueprint::FocusMode fm;
			int i=0;

			if(focus.to==Blueprint::Focus_None) {
				if(Mapping[gm(o, focus)])
					groups[i++]=Mapping[gm(o, focus)];

				if(focus.from!=Focus_None) {
					if(Mapping[gm(o, fm())])
						groups[i++]=Mapping[gm(o, fm())];
				}
			}
			else {
				if(Mapping[gm(o, focus)])
					groups[i++]=Mapping[gm(o, focus)];

				if(Mapping[gm(o, focus.swap())])
					groups[i++]=Mapping[gm(o, focus.swap())];

				if(Mapping[gm(o, fm())])
					groups[i++]=Mapping[gm(o, fm())];
			}

			for(;i<3;i++)
				groups[i]=NULL;
		}

	}

}}
