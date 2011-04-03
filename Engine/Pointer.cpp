#include "Pointer.h"
#include "../Utils/Collection.h"
#include "../Utils/LinkedList.h"
#include "GraphicLayers.h"
#include "GGEMain.h"
#include "../Resource/FolderResource.h"
#include "../Resource/AnimationResource.h"
#include "../Resource/DataResource.h"
#include "../Resource/GRE.h"
#include "OS.h"

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

		Pointer *pointer=ActivePointers.getOrderedLastItem();
		if(pointer==NULL)
			pointer=BasePointer;


		pointer->Image->Draw(PointerLayer, pnt-pointer->Hotspot);
	}

	void PointerCollection::Initialize(GGEMain &Main) {
		PointerLayer=dynamic_cast<Basic2DLayer*>(Main.Add( new Basic2DLayer(0, 0, Main.getWidth(), Main.getHeight()) , -100 ));

		os::window::Activated.Register(this, &PointerCollection::Window_Activate);
		os::window::Deactivated.Register(this, &PointerCollection::Window_Deactivate);

		Main.BeforeRenderEvent.Register(this, &PointerCollection::Draw);

		os::HidePointer();
		PointerLayer->isVisible=false;
	}

	void PointerCollection::Fetch(FolderResource *Folder) {
		DataResource *data=Folder->asData(0);
		
		utils::LinkedListIterator<ResourceBase> it=Folder->Subitems;
		ResourceBase *resource;
		resource=it;

		int i=0;
		while(resource=it) {
			if(resource->getGID()==GID_ANIMATION) {
				AnimationResource *anim=dynamic_cast<AnimationResource *>(resource);
				utils::Collection<Pointer, 10>::Add( new Pointer(anim->getAnimation(), data->getPoint(i+1).x, data->getPoint(i+1).y, (Pointer::PointerTypes)data->getInt(i)) );
			} else if(resource->getGID()==GID_IMAGE) {
				ImageResource *img=dynamic_cast<ImageResource *>(resource);
				utils::Collection<Pointer, 10>::Add( new Pointer(img, data->getPoint(i+1).x, data->getPoint(i+1).y, (Pointer::PointerTypes)data->getInt(i)) );
			}

			i+=2;
		}

		if(i>0)
			BasePointer=(*this)[0];
	}

	Pointer *PointerCollection::Add(Buffered2DGraphic *pointer, Point Hotspot, Pointer::PointerTypes Type) {
		Pointer *ret=new Pointer(pointer, Hotspot.x, Hotspot.y, Type);
		utils::Collection<Pointer, 10>::Add( ret );
		return ret;
	}

	int PointerCollection::Set(Pointer *Pointer) {
		return reinterpret_cast<int>(ActivePointers.AddItem(Pointer, ActivePointers.HighestOrder()+1));
	}

	int PointerCollection::Set(Pointer::PointerTypes Type) {
		if(Type==Pointer::None)
			return Set(BasePointer);
		this->ResetIteration();
		Pointer *pointer;
		while(pointer=this->next()) {
			if(pointer->Type==Type)
				return Set(pointer);
		}

		return Set(BasePointer);
	}

	void PointerCollection::Reset(int StackNo) {
		ActivePointers.Remove(reinterpret_cast<utils::LinkedListItem<Pointer>*>(StackNo));
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