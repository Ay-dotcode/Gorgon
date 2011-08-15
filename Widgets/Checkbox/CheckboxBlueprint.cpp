#include "CheckboxBlueprint.h"
#include "..\..\Engine\Pointer.h"
#include "..\..\Resource\ResourceFile.h"

using namespace gge::resource;
using namespace std;
using namespace gge::animation;
using namespace gge::graphics;

namespace gge { namespace widgets {

	//only service classes will be here
	namespace checkbox {

		Blueprint::Line *LoadLine(resource::File& File, std::istream &Data, int Size) {
			Blueprint::Line *bp=new Blueprint::Line;

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
				else if(gid==GID::Checkbox_Line_Props) {
					bp->border.Load(Data);
					bp->Contents		=(Blueprint::LineContents)ReadFrom<int>(Data);
					bp->WidthMode	=(Blueprint::SizingMode  )ReadFrom<int>(Data);
					bp->HeightMode  =(Blueprint::HeightType  )ReadFrom<int>(Data);
					ReadFrom(Data, bp->Height);
					bp->Align		=(Alignment::Type        )ReadFrom<int>(Data);

					EatChunk(Data, size-(1 * 8 + 5 * 4));
				}
				else {
					ResourceBase *res=File.LoadObject(Data, gid,size);
					if(res)
						bp->Subitems.Add(res, bp->Subitems.HighestOrder()+1);
				}
			}

			return bp;
		}

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
				else if(gid==GID::Checkbox_Element_Props) {
					int ptarget=int(Data.tellg());

					ReadFrom(Data, bp->Duration);
					bp->border.Load(Data);
					ReadFrom<int>(Data);
					int sz=ReadFrom<int>(Data);
					bp->font=Font::Load(File, Data, sz);
					bp->lines[0].Load(Data);
					bp->lines[1].Load(Data);
					bp->lines[2].Load(Data);
					bp->symbol.Load(Data);
					bp->symbolplaceholder.Load(Data);
					bp->textplaceholder.Load(Data);
					bp->iconplaceholder.Load(Data);
					bp->sound.Load(Data);
					bp->overlay.Load(Data);

					Data.seekg(ptarget+size);
				}
				else if(gid==GID::Checkbox_Line) {
					Blueprint::Line *l=LoadLine(File, Data, size);

					bp->Subitems.Add(l, bp->Subitems.HighestOrder()+1);
				}
				else {
					ResourceBase *res=File.LoadObject(Data, gid,size);
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
				else if(gid==GID::Checkbox_Group_Props) {
					bp->State = (Blueprint::StateMode)ReadFrom<int>(Data);
					bp->Focus = (Blueprint::FocusType)ReadFrom<int>(Data);

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

					EatChunk(Data, size-(4 * 2 + 8 * 12));
				}
				else if(gid==GID::Checkbox_Element) {
					Blueprint::Element *elm=LoadElement(File, Data, size);

					bp->Subitems.Add(elm, bp->Subitems.HighestOrder()+1);
				}
				else {
					ResourceBase *res=File.LoadObject(Data, gid,size);
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
				else if(gid==GID::Checkbox_Props) {
					bp->Pointer=(Pointer::PointerType	)ReadFrom<int>(Data);
					bp->States =(Blueprint::StateNumbers)ReadFrom<int>(Data);
					ReadFrom(Data, bp->DefaultSize);
					ReadFrom(Data, bp->AlphaAnimation);

					EatChunk(Data, size-(4*3 + 8*1));
				}
				else if(gid==GID::Checkbox_Group) {
					Blueprint::Group *grp=LoadGroup(File, Data, size);
					
					bp->Subitems.Add(grp, bp->Subitems.HighestOrder()+1);
				}
				else {
					ResourceBase *res=File.LoadObject(Data, gid,size);
					if(res)
						bp->Subitems.Add(res, bp->Subitems.HighestOrder()+1);
				}
			}

			for(auto it=bp->Subitems.First();it.isValid();it.Next()) {
				Blueprint::Group &g=dynamic_cast<Blueprint::Group &>(*it);

				bp->Mapping[g] = &g;
			}


			return bp;
		}

		Blueprint::AnimationInfo Blueprint::HasStyleAnimation(FocusMode f, StateMode s, StyleMode style) const {
			Blueprint::AnimationInfo ret;

			ret=hasstyleanimation(FocusMode(),StateMode(),style);
			if(!ret)
				return ret;

			ret=hasstyleanimation(FocusMode(),s,style);
			if(!ret)
				return ret;

			ret=hasstyleanimation(f,StateMode(),style);
			if(!ret)
				return ret;

			if(f.to!=Blueprint::FT_None) {
				ret=hasstyleanimation(f.swap(),StateMode(),style);
				if(!ret)
					return ret;
			}
			
			if(s.to!=0) {
				ret=hasstyleanimation(FocusMode(),s.swap(),style);
				if(!ret)
					return ret;
			}

			ret=hasstyleanimation(f,s,style);
			if(!ret)
				return ret;

			return Missing;
		}

		Blueprint::AnimationInfo Blueprint::HasStateAnimation(StateMode state) const {
			if(Mapping[GroupMode(FocusMode(),state)])
				return Blueprint::Forward;

			state.swap();
			if(Mapping[GroupMode(FocusMode(),state)])
				return Blueprint::Backward;

			return Blueprint::Missing;
		}

		Blueprint::AnimationInfo Blueprint::HasFocusAnimation(FocusMode focus) const {
			if(Mapping[GroupMode(focus,StateMode())])
				return Blueprint::Forward;

			focus.swap();
			if(Mapping[GroupMode(focus,StateMode())])
				return Blueprint::Backward;

			return Blueprint::Missing;
		}

		void Blueprint::GetAlternatives(Group** &groups, FocusMode focus, StateMode state) const {
			typedef Blueprint::GroupMode gm;
			typedef Blueprint::FocusMode fm;
			typedef Blueprint::StateMode sm;
			int i=0;

			if(focus.to==Blueprint::FT_None && state.to==0) {
				//???????????????????????????????????????????
				//if(Mapping[gm(focus, state)])
				//	groups[i++]=Mapping[gm(focus, state)];

				if(focus.from!=Blueprint::NotFocused) { //means we will do it with fm(), sm()
					if(Mapping[gm(focus, sm())])
						groups[i++]=Mapping[gm(focus, sm())];
				}

				if(state.from!=1) { //means we will d0 it with fm(), sm()
					if(Mapping[gm(fm(), state)])
						groups[i++]=Mapping[gm(fm(), state)];
				}

				if(Mapping[gm(fm(), sm())])
					groups[i++]=Mapping[gm(fm(), sm())];
			}
			else if(focus.to==Blueprint::FT_None) {
				if(Mapping[gm(focus, sm())])
					groups[i++]=Mapping[gm(focus, sm())];

				if(Mapping[gm(fm(), state)])
					groups[i++]=Mapping[gm(fm(), state)];

				if(Mapping[gm(fm(), state.swap())])
					groups[i++]=Mapping[gm(fm(), state.swap())];

				if(focus.from!=Blueprint::NotFocused) { //means we already did it with focus, sm()
					if(Mapping[gm(fm(), sm())])
						groups[i++]=Mapping[gm(fm(), sm())];
				}
			}
			else if(state.to==0) {
				if(Mapping[gm(focus, sm())])
					groups[i++]=Mapping[gm(focus, sm())];

				if(Mapping[gm(fm(), state)])
					groups[i++]=Mapping[gm(fm(), state)];

				if(Mapping[gm(focus.swap(), sm())])
					groups[i++]=Mapping[gm(focus.swap(), sm())];

				if(state.from!=1) { //means we already did it with fm(), state
					if(Mapping[gm(fm(), sm())])
						groups[i++]=Mapping[gm(fm(), sm())];
				}
			}
			else {
				if(Mapping[gm(focus, sm())])
					groups[i++]=Mapping[gm(focus, sm())];

				if(Mapping[gm(fm(), state)])
					groups[i++]=Mapping[gm(fm(), state)];

				if(Mapping[gm(focus.swap(), sm())])
					groups[i++]=Mapping[gm(focus.swap(), sm())];

				if(Mapping[gm(fm(), state.swap())])
					groups[i++]=Mapping[gm(fm(), state.swap())];

				if(Mapping[gm(fm(), sm())])
					groups[i++]=Mapping[gm(fm(), sm())];
			}

			for(;i<5;i++)
				groups[i]=NULL;
		}



		Blueprint::GroupMode::GroupMode(const Group &g) : focus(g.Focus), state(g.State) {

		}

		void Blueprint::Group::Prepare(GGEMain &main, resource::File &file) {
			resource::ResourceBase::Prepare(main,file);

			Normal			=dynamic_cast<Blueprint::Element*>(file.FindObject(normal));
			Mapping[Blueprint::Normal][Blueprint::YT_None]=Normal;
			
			Hover			=dynamic_cast<Blueprint::Element*>(file.FindObject(hover));
			Mapping[Blueprint::Hover][Blueprint::YT_None]=Hover;

			Down			=dynamic_cast<Blueprint::Element*>(file.FindObject(down));
			Mapping[Blueprint::Down][Blueprint::YT_None]=Down;

			Disabled		=dynamic_cast<Blueprint::Element*>(file.FindObject(disabled));
			Mapping[Blueprint::Disabled][Blueprint::YT_None]=Disabled;


			NormalToHover	=dynamic_cast<Blueprint::Element*>(file.FindObject(normaltohover));
			Mapping[Blueprint::Normal][Blueprint::Hover]=NormalToHover;

			NormalToDown	=dynamic_cast<Blueprint::Element*>(file.FindObject(normaltodown));
			Mapping[Blueprint::Normal][Blueprint::Down]=NormalToDown;

			NormalToDisabled=dynamic_cast<Blueprint::Element*>(file.FindObject(normaltodisabled));
			Mapping[Blueprint::Normal][Blueprint::Disabled]=NormalToDisabled;

			HoverToDown		=dynamic_cast<Blueprint::Element*>(file.FindObject(hovertodown));
			Mapping[Blueprint::Hover][Blueprint::Down]=HoverToDown;


			HoverToNormal	=dynamic_cast<Blueprint::Element*>(file.FindObject(hovertonormal));
			Mapping[Blueprint::Hover][Blueprint::Normal]=HoverToNormal;

			DownToNormal	=dynamic_cast<Blueprint::Element*>(file.FindObject(downtonormal));
			Mapping[Blueprint::Down][Blueprint::Normal]=DownToNormal;

			DisabledToNormal=dynamic_cast<Blueprint::Element*>(file.FindObject(disabledtonormal));
			Mapping[Blueprint::Disabled][Blueprint::Normal]=DisabledToNormal;

			DownToHover		=dynamic_cast<Blueprint::Element*>(file.FindObject(downtohover));
			Mapping[Blueprint::Down][Blueprint::Hover]=DownToHover;
		}


		void Blueprint::Element::Prepare(GGEMain &main, resource::File &file) {
			resource::ResourceBase::Prepare(main,file);

			Border				=dynamic_cast<BorderDataResource*>(file.FindObject(border));
			Symbol				=dynamic_cast<resource::ResizableObjectProvider*>(file.FindObject(symbol));
			Font				=new gge::Font((gge::Font)font);
			SymbolPlaceholder	=dynamic_cast<Placeholder*>(file.FindObject(symbolplaceholder));
			TextPlaceholder		=dynamic_cast<Placeholder*>(file.FindObject(textplaceholder));
			IconPlaceholder		=dynamic_cast<Placeholder*>(file.FindObject(iconplaceholder));
			Sound				=dynamic_cast<resource::SoundResource*>(file.FindObject(sound));
			Overlay				=dynamic_cast<BorderDataResource*>(file.FindObject(overlay));

			for(int i=0;i<3;i++) {
				Lines[i]			=dynamic_cast<Line*>(file.FindObject(lines[i]));
			}
		}


		void Blueprint::Line::Prepare(GGEMain &main, resource::File &file) {
			resource::ResourceBase::Prepare(main,file);

			Border				=dynamic_cast<BorderDataResource*>(file.FindObject(border));
		}

	}

}}
