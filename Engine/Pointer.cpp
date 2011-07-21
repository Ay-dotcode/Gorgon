#include "Pointer.h"
#include "../Utils/Collection.h"
#include "../Utils/SortedCollection.h"
#include "GraphicLayers.h"
#include "GGEMain.h"
#include "../Resource/FolderResource.h"
#include "../Resource/AnimationResource.h"
#include "../Resource/DataResource.h"
#include "../Resource/GRE.h"
#include "OS.h"

using namespace gge::resource;
using namespace gge::utils;

namespace gge {

	void PointerCollection::Window_Activate() {
		this->Show();
	}

	void PointerCollection::Window_Deactivate() {
		this->Hide();
	}

	void PointerCollection::Draw(GGEMain &caller) {
		if(!BasePointer || !PointerVisible)
			return;

		Point pnt;
		pnt=os::input::getMousePosition(caller.getWindow());

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
		
		if(ActivePointers.getCount()==0)
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
		PointerLayer->isVisible=false;
	}

	void PointerCollection::Fetch(FolderResource *Folder) {
		DataResource *data=Folder->asData(0);

		int i=0;
		for(SortedCollection<ResourceBase>::Iterator resource=Folder->Subitems.First();resource.isValid();resource.Next()) {
			if(resource->getGID()==GID::Animation) {
				AnimationResource *anim=dynamic_cast<AnimationResource *>(resource.CurrentPtr());
				utils::Collection<Pointer, 10>::Add( new Pointer(anim->getAnimation(), data->getPoint(i+1).x, data->getPoint(i+1).y, (Pointer::PointerTypes)data->getInt(i)) );
			} else if(resource->getGID()==GID::Image) {
				ImageResource *img=dynamic_cast<ImageResource *>(resource.CurrentPtr());
				utils::Collection<Pointer, 10>::Add( new Pointer(img, data->getPoint(i+1).x, data->getPoint(i+1).y, (Pointer::PointerTypes)data->getInt(i)) );
			}

			i+=2;
		}

		if(i>0)
			BasePointer=&(*this)[0];
	}

	Pointer *PointerCollection::Add(graphics::Buffered2DGraphic *pointer, Point Hotspot, Pointer::PointerTypes Type) {
		Pointer *ret=new Pointer(pointer, Hotspot.x, Hotspot.y, Type);
		utils::Collection<Pointer, 10>::Add( ret );
		
		if(!BasePointer)
			BasePointer=ret;

		return ret;
	}

	PointerCollection::Token PointerCollection::Set(Pointer *Pointer) {
		return reinterpret_cast<Token>(&ActivePointers.Add(Pointer, ActivePointers.HighestOrder()+1));
	}

	PointerCollection::Token PointerCollection::Set(Pointer::PointerTypes Type) {
		if(Type==Pointer::None)
			return Set(BasePointer);
		for(Collection<Pointer,10>::Iterator pointer=this->First();pointer.isValid();pointer.Next()) {
			if(pointer->Type==Type)
				return Set(pointer);
		}

		return Set(BasePointer);
	}

	void PointerCollection::Reset(PointerCollection::Token StackNo) {
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

		PointerLayer->isVisible=true;
	}

	void PointerCollection::Hide() {
		PointerVisible=false;

		PointerLayer->isVisible=false;
	}

	PointerCollection Pointers;

}