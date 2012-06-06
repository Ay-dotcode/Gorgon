#include "Pointer.h"
#include "../Utils/Collection.h"
#include "../Utils/SortedCollection.h"
#include "GraphicLayers.h"
#include "GGEMain.h"
#include "../Resource/Folder.h"
#include "../Resource/Animation.h"
#include "../Resource/DataArray.h"
#include "../Resource/GRE.h"
#include "../Resource/Pointer.h"
#include "OS.h"
#include "Animation.h"

using namespace gge::resource;
using namespace gge::utils;
using namespace gge::animation;

namespace gge {

	void PointerCollection::Window_Activate() {
		//this->Show();
	}

	void PointerCollection::Window_Deactivate() {
		//this->Hide();
	}

	void PointerCollection::Draw(GGEMain &caller) {
		if(!BasePointer || !PointerVisible)
			return;

		Point pnt;
		pnt=input::mouse::CurrentPoint;

		PointerLayer->Clear();

		if(pnt.x<0 || pnt.y<0 || pnt.x>caller.getWidth() || pnt.y>caller.getHeight()) {
			os::ShowPointer();
			OSPointerVisible=true;
			return;
		} else if(OSPointerVisible) {
			OSPointerVisible=false;
			os::HidePointer();
		}

		Pointer *pointer;
		
		if(ActivePointers.GetCount()==0)
			pointer=BasePointer;
		else
			pointer=ActivePointers.LastItem().GetPtr();


		pointer->Image->Draw(PointerLayer, pnt-pointer->Hotspot);
	}

	void PointerCollection::Initialize(GGEMain &Main, int LayerOrder) {
		PointerLayer=new graphics::Basic2DLayer(0, 0, Main.getWidth(), Main.getHeight());
		Main.Add( PointerLayer , LayerOrder );

		os::window::Activated.Register(this, &PointerCollection::Window_Activate);
		os::window::Deactivated.Register(this, &PointerCollection::Window_Deactivate);

		Main.BeforeRenderEvent.Register(this, &PointerCollection::Draw);

		os::HidePointer();
		PointerLayer->IsVisible=false;
	}

	void PointerCollection::Fetch(Folder *Folder) {
		if(Folder->GetCount()==0) return;

		if(Folder->GetItem(0)->GetGID()==GID::Data) {
			DataArray *data=Folder->asData(0);

			int i=0;
			for(SortedCollection<resource::Base>::Iterator resource=Folder->Subitems.First()+1;resource.IsValid();resource.Next()) {
				RectangularGraphic2DSequenceProvider *anim=dynamic_cast<RectangularGraphic2DSequenceProvider *>(resource.CurrentPtr());
				utils::Collection<Pointer, 10>::Add( new Pointer(&anim->CreateAnimation(), data->getPoint(i+1).x, data->getPoint(i+1).y, (Pointer::PointerType)data->getInt(i)) );

				i+=2;
			}

			if(i>0)
				BasePointer=&(*this)[0];
		}
		else {
			for(SortedCollection<resource::Base>::Iterator resource=Folder->Subitems.First();resource.IsValid();resource.Next()) {
				resource::Pointer *ptr=dynamic_cast<resource::Pointer *>(resource.CurrentPtr());
				utils::Collection<Pointer, 10>::Add( new Pointer(ptr->CreateAnimation(true), ptr->Hotspot, ptr->Type) );
			}

			BasePointer=&(*this)[0];
		}
	}

	Pointer *PointerCollection::Add(graphics::RectangularGraphic2D *pointer, Point Hotspot, Pointer::PointerType Type) {
		Pointer *ret=new Pointer(pointer, Hotspot.x, Hotspot.y, Type);
		utils::Collection<Pointer, 10>::Add( ret );
		
		if(!BasePointer)
			BasePointer=ret;

		return ret;
	}

	PointerCollection::Token PointerCollection::Set(Pointer *Pointer) {
		return reinterpret_cast<Token>(&ActivePointers.Add(Pointer, ActivePointers.HighestOrder()+1));
	}

	PointerCollection::Token PointerCollection::Set(Pointer::PointerType Type) {
		if(Type==Pointer::None)
			return Set(BasePointer);
		for(Collection<Pointer,10>::Iterator pointer=this->First();pointer.IsValid();pointer.Next()) {
			if(pointer->Type==Type)
				return Set(pointer.CurrentPtr());
		}

		return Set(BasePointer);
	}

	void PointerCollection::Reset(PointerCollection::Token StackNo) {
		if(StackNo==0) return;

		ActivePointers.Remove(*reinterpret_cast<utils::SortedCollection<Pointer>::Wrapper*>(StackNo));
	}

	void PointerCollection::ChangeBase(Pointer *Pointer) {
		BasePointer=Pointer;
	}

	void PointerCollection::Show() {
		PointerVisible=true;
		os::HidePointer();
		os::HidePointer();
		os::HidePointer();

		PointerLayer->IsVisible=true;
	}

	void PointerCollection::Hide() {
		PointerVisible=false;

		PointerLayer->IsVisible=false;
	}

	PointerCollection Pointers;

}