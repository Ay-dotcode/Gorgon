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
	Collection<Pointer> Pointers;
	LinkedList<Pointer> ActivePointers;
	Basic2DLayer *PointerLayer=NULL;
	Pointer *BasePointer=NULL;
	bool PointerVisible=false;
	bool OSPointerVisible=true;

	void Pointer_Window_Activate(empty_event_params p, Empty &caller, Any data, string eventname) {
		ShowPointer();
	}

	void Pointer_Window_Deactivate(empty_event_params p, Empty &caller, Any data, string eventname) {
		HidePointer();
	}

	void Pointer_Draw(empty_event_params p, GGEMain &caller, Any data, string eventname) {
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

	void InitializePointer(GGEMain *Main) {
		PointerLayer=(Basic2DLayer*)Main->Add( new Basic2DLayer(0, 0, Main->getWidth(), Main->getHeight()) , -100 );

		WindowActivateEvent.Register(Pointer_Window_Activate);
		WindowDeactivateEvent.Register(Pointer_Window_Deactivate);

		Main->BeforeRenderEvent.Register(Pointer_Draw);

		HideOSPointer();
		PointerLayer->isVisible=false;
	}

	void FetchPointers(FolderResource *Folder) {
		DataResource *data=Folder->asData(0);
		
		LinkedListIterator<ResourceBase> it=Folder->Subitems;
		ResourceBase *resource;
		resource=it;

		int i=0;
		while(resource=it) {
			if(resource->getGID()==GID_ANIMATION) {
				AnimationResource *anim=dynamic_cast<AnimationResource *>(resource);
				Pointers.Add( new Pointer(anim->getAnimation(), data->getPoint(i+1).x, data->getPoint(i+1).y, (Pointer::PointerTypes)data->getInt(i)) );
			} else if(resource->getGID()==GID_IMAGE) {
				ImageResource *img=dynamic_cast<ImageResource *>(resource);
				Pointers.Add( new Pointer(img, data->getPoint(i+1).x, data->getPoint(i+1).y, (Pointer::PointerTypes)data->getInt(i)) );
			}

			i+=2;
		}

		if(i>0)
			BasePointer=Pointers[0];
	}

	Pointer *AddPointer(Buffered2DGraphic *pointer, Point Hotspot, Pointer::PointerTypes Type) {
		Pointer *ret=new Pointer(pointer, Hotspot.x, Hotspot.y, Type);
		Pointers.Add( ret );
		return ret;
	}

	void RemovePointer(Pointer *Pointer) {
		Pointers.Remove(Pointer);
	}

	int SetPointer(Pointer *Pointer) {
		return reinterpret_cast<int>(ActivePointers.AddItem(Pointer, ActivePointers.HighestOrder()+1));
	}

	int SetPointer(Pointer::PointerTypes Type) {
		if(Type==Pointer::None)
			return SetPointer(BasePointer);
		Pointers.ResetIteration();
		Pointer *pointer;
		while(pointer=Pointers.next()) {
			if(pointer->Type==Type)
				return SetPointer(pointer);
		}

		return SetPointer(BasePointer);
	}

	void ResetPointer(int StackNo) {
		ActivePointers.Remove(reinterpret_cast<LinkedListItem<Pointer>*>(StackNo));
	}

	void ChangeBasePointer(Pointer *Pointer) {
		BasePointer=Pointer;
	}

	void ShowPointer() {
		PointerVisible=true;
		HideOSPointer();
		HideOSPointer();
		HideOSPointer();

		PointerLayer->isVisible=true;
	}

	void HidePointer() {
		PointerVisible=false;

		PointerLayer->isVisible=false;
	}

}