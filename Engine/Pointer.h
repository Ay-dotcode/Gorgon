#pragma once

#include "../Utils/GGE.h"
#include "Graphics.h"

namespace gre { class FolderResource; }

using namespace gre;

namespace gge {
	class GGEMain;

	////Pointer types
	enum PointerTypes {
		////No pointer is selected or using default
		None=0,
		////Arrow / Pointer
		Arrow=1,
		////Wait / Hourglass
		Wait=2,
		////No / Not allowed
		No=3,
		////Text / Beam pointer
		Text=4,
		////Hand pointer
		Hand=5,
		////Drag / Closed hand pointer
		Drag=6
	};

	class Pointer {
	public:
		////The image of the pointer
		Buffered2DGraphic *Image;
		////Point of click
		int HotspotX;
		////Point of click
		int HotspotY;
		////Type of the pointer
		PointerTypes Type;

		////Initializes a pointer
		Pointer(Buffered2DGraphic *pointer, int HotspotX, int HotspotY, PointerTypes Type) {
			this->Image=pointer;
			this->HotspotX=HotspotX;
			this->HotspotY=HotspotY;
			this->Type=Type;
		}
	};

	////Initializes Pointer Subsystem
	void InitializePointer(GGEMain *Main);
	////Fetches list of pointers from a given folder resource. The given folder shall contain
	/// a data file as the first item containing two entries per pointer. First entry must be
	/// the Type(integer) ranging 0-6, second is Hotspot(point). Every pointer should be either
	/// animation or image resource
	void FetchPointers(FolderResource *Folder);
	////Adds a pointer to the list of pointers
	Pointer *AddPointer(Buffered2DGraphic *Pointer, Point Hotspot, PointerTypes Type=None);
	////Removes a pointer from the list
	void RemovePointer(Buffered2DGraphic *Pointer);
	////Sets the given pointer as current one, this operation should be revered by
	/// using reset pointer with the returned integer
	int SetPointer(Pointer *Pointer);
	////Sets the given pointer as current one, this operation should be revered by
	/// using reset pointer with the returned integer
	int SetPointer(PointerTypes Type);
	////This function changes base pointer to the given one, it is best to use this function
	/// once at the startup. This function or FetchFolders function should be called before 
	/// calling show pointer function
	void ChangeBasePointer(Pointer *Pointer);
	////Removes a given stack no from pointer display list. The displayed pointer is always
	/// the one at the top. But any pointer can be removed from the list without the requirement
	/// of being on top. 
	void ResetPointer(int StackNo);
	////Shows current pointer, ChangeBasePointer or FetchPointers function should be called before
	/// calling this function
	void ShowPointer();
	////Hides current pointer
	void HidePointer();
}