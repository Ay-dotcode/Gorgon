#pragma once

#include "../Utils/GGE.h"
#include "../Utils/LinkedList.h"
#include "Graphics.h"

namespace gre { class FolderResource; }

using namespace gre;

namespace gge {
	class GGEMain;
	class Basic2DLayer;


	class Pointer {
	public:

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

		////The image of the pointer
		Buffered2DGraphic *Image;
		////Point of click
		Point Hotspot;
		////Type of the pointer
		PointerTypes Type;

		////Initializes a pointer
		Pointer(Buffered2DGraphic *pointer, int HotspotX, int HotspotY, PointerTypes Type) {
			this->Image=pointer;
			this->Hotspot.x=HotspotX;
			this->Hotspot.y=HotspotY;
			this->Type=Type;
		}

		////Initializes a pointer
		Pointer(Buffered2DGraphic &pointer, Point Hotspot, PointerTypes Type) {
			this->Image=&pointer;
			this->Hotspot=Hotspot;
			this->Type=Type;
		}
	};


	class PointerCollection : public utils::Collection<Pointer, 10> {
	public:
		////Initializes Pointer Subsystem
		void Initialize(GGEMain &Main);
		////Fetches list of pointers from a given folder resource. The given folder shall contain
		/// a data file as the first item containing two entries per pointer. First entry must be
		/// the Type(integer) ranging 0-6, second is Hotspot(point). Every pointer should be either
		/// animation or image resource
		void Fetch(FolderResource *Folder);
		void Fetch(FolderResource &Folder) { Fetch(&Folder); }
		////Adds a pointer to the list of pointers
		Pointer *Add(Buffered2DGraphic *Pointer, Point Hotspot=Point(2,2), Pointer::PointerTypes Type=Pointer::None);
		////Adds a pointer to the list of pointers
		Pointer &Add(Buffered2DGraphic &Pointer, Point Hotspot=Point(2,2), Pointer::PointerTypes Type=Pointer::None) {
			return *Add(&Pointer, Hotspot, Type);
		}
		////Sets the given pointer as current one, this operation should be revered by
		/// using reset pointer with the returned integer
		int Set(Pointer *Pointer);
		////Sets the given pointer as current one, this operation should be revered by
		/// using reset pointer with the returned integer
		int Set(Pointer::PointerTypes Type);
		////This function changes base pointer to the given one, it is best to use this function
		/// once at the startup. This function or FetchFolders function should be called before 
		/// calling show pointer function
		void ChangeBase(Pointer *Pointer);
		void ChangeBase(Pointer &Pointer) { ChangeBase(&Pointer); }
		////Removes a given stack no from pointer display list. The displayed pointer is always
		/// the one at the top. But any pointer can be removed from the list without the requirement
		/// of being on top. 
		void Reset(int StackNo);
		////Shows current pointer, ChangeBasePointer or FetchPointers function should be called before
		/// calling this function
		void Show();
		////Hides current pointer
		void Hide();

		PointerCollection() : PointerLayer(NULL), BasePointer(NULL), PointerVisible(false), OSPointerVisible(true) { }

	protected:
		utils::LinkedList<Pointer> ActivePointers;
		Basic2DLayer *PointerLayer;
		Pointer *BasePointer;
		bool PointerVisible;
		bool OSPointerVisible;


		//Event handlers
		void Window_Activate();
		void Window_Deactivate();
		void Draw(GGEMain &caller);
	};

	extern PointerCollection Pointers;

}