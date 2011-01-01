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

	void Cpointers::Window_Activate() {
		this->Show();
	}

	void Cpointers::Window_Deactivate() {
		this->Hide();
	}

	void Cpointers::Draw(GGEMain &caller) {
		if(!BasePointer || !PointerVisible)
			return;

		Point pnt;
		pnt=getMousePosition(caller.getWindow());

		PointerLayer->Clear();

		if(pnt.x<0 || pnt.y<0 || pnt.x>caller.getWidth() || pnt.y>caller.getHeight()) {
			ShowOSPointer();
			OSPointerVisible=true;
			return;
		} else if(OSPointerVisible) {
			OSPointerVisible=false;
			HideOSPointer();
		}

		Pointer *pointer=ActivePointers.getOrderedLastItem();
		if(pointer==NULL)
			pointer=BasePointer;

		int x,y;

		x=pnt.x-pointer->HotspotX;
		y=pnt.y-pointer->HotspotY;

		pointer->Image->Draw(PointerLayer, x, y);
	}

	void Cpointers::Initialize(GGEMain &Main) {
		PointerLayer=(Basic2DLayer*)Main.Add( new Basic2DLayer(0, 0, Main.getWidth(), Main.getHeight()) , -100 );

		WindowActivateEvent.Register(this, &Cpointers::Window_Activate);
		WindowDeactivateEvent.Register(this, &Cpointers::Window_Deactivate);

		Main.BeforeRenderEvent.Register(this, &Cpointers::Draw);

		HideOSPointer();
		PointerLayer->isVisible=false;
	}

	void Cpointers::Fetch(FolderResource *Folder) {
		DataResource *data=Folder->asData(0);
		
		LinkedListIterator<ResourceBase> it=Folder->Subitems;
		ResourceBase *resource;
		resource=it;

		int i=0;
		while(resource=it) {
			if(resource->getGID()==GID_ANIMATION) {
				AnimationResource *anim=dynamic_cast<AnimationResource *>(resource);
				Collection<Pointer, 10>::Add( new Pointer(anim->getAnimation(), data->getPoint(i+1).x, data->getPoint(i+1).y, (Pointer::PointerTypes)data->getInt(i)) );
			} else if(resource->getGID()==GID_IMAGE) {
				ImageResource *img=dynamic_cast<ImageResource *>(resource);
				Collection<Pointer, 10>::Add( new Pointer(img, data->getPoint(i+1).x, data->getPoint(i+1).y, (Pointer::PointerTypes)data->getInt(i)) );
			}

			i+=2;
		}

		if(i>0)
			BasePointer=(*this)[0];
	}

	Pointer *Cpointers::Add(Buffered2DGraphic *pointer, Point Hotspot, Pointer::PointerTypes Type) {
		Pointer *ret=new Pointer(pointer, Hotspot.x, Hotspot.y, Type);
		Collection<Pointer, 10>::Add( ret );
		return ret;
	}

	int Cpointers::Set(Pointer *Pointer) {
		return reinterpret_cast<int>(ActivePointers.AddItem(Pointer, ActivePointers.HighestOrder()+1));
	}

	int Cpointers::Set(Pointer::PointerTypes Type) {
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

	void Cpointers::Reset(int StackNo) {
		ActivePointers.Remove(reinterpret_cast<LinkedListItem<Pointer>*>(StackNo));
	}

	void Cpointers::ChangeBase(Pointer *Pointer) {
		BasePointer=Pointer;
	}

	void Cpointers::Show() {
		PointerVisible=true;
		HideOSPointer();
		HideOSPointer();
		HideOSPointer();

		PointerLayer->isVisible=true;
	}

	void Cpointers::Hide() {
		PointerVisible=false;

		PointerLayer->isVisible=false;
	}

	Cpointers Pointers;

}