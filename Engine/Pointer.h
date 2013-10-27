#pragma once

#include <cstdint>

#include "GGE.h"
#include "../Utils/SortedCollection.h"
#include "Graphics.h"
#include "Graphic2D.h"

namespace gge {
	class GGEMain;
	namespace graphics { class Basic2DLayer; }
	namespace resource { class Folder; class Pointer; }

	class Pointer {
	public:

		////Pointer types
		enum PointerType {
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
		graphics::Graphic2D *Image;
		////Point of click
		utils::Point Hotspot;
		////Type of the pointer
		PointerType Type;

		////Initializes a pointer
		Pointer(graphics::Graphic2D *pointer, int HotspotX, int HotspotY, PointerType Type) {
			this->Image=pointer;
			this->Hotspot.x=HotspotX;
			this->Hotspot.y=HotspotY;
			this->Type=Type;
		}

		////Initializes a pointer
		Pointer(graphics::Graphic2D *pointer, utils::Point Hotspot, PointerType Type) {
			this->Image=pointer;
			this->Hotspot=Hotspot;
			this->Type=Type;
		}

		////Initializes a pointer
		Pointer(graphics::Graphic2D &pointer, int HotspotX, int HotspotY, PointerType Type) {
			this->Image=&pointer;
			this->Hotspot.x=HotspotX;
			this->Hotspot.y=HotspotY;
			this->Type=Type;
		}

		////Initializes a pointer
		Pointer(graphics::Graphic2D &pointer, utils::Point Hotspot, PointerType Type) {
			this->Image=&pointer;
			this->Hotspot=Hotspot;
			this->Type=Type;
		}

		Pointer(resource::Pointer &pointer);

		~Pointer();
	};


	class PointerCollection : public utils::Collection<Pointer, 10> {
	public:

		typedef std::intptr_t Token;
		static const Token NullToken = 0;


		////Initializes Pointer Subsystem
		void Initialize(GGEMain &Main, int LayerOrder=-100);
		////Fetches list of pointers from a given folder resource. The given folder shall contain
		/// a data file as the first item containing two entries per pointer. First entry must be
		/// the Type(integer) ranging 0-6, second is Hotspot(point). Every pointer should be either
		/// animation or image resource
		void Fetch(resource::Folder *Folder);
		void Fetch(resource::Folder &Folder) { Fetch(&Folder); }
		////Adds a pointer to the list of pointers
		void Add(Pointer &pointer);
		////Adds a pointer to the list of pointers
		void Add(Pointer *pointer) { if(pointer) Add(*pointer); }
		////Adds a pointer to the list of pointers
		Pointer *Add(graphics::RectangularGraphic2D *Pointer, utils::Point Hotspot=utils::Point(2,2), Pointer::PointerType Type=Pointer::None);
		////Adds a pointer to the list of pointers
		Pointer &Add(graphics::RectangularGraphic2D &Pointer, utils::Point Hotspot=utils::Point(2,2), Pointer::PointerType Type=Pointer::None) {
			return *Add(&Pointer, Hotspot, Type);
		}
		////Sets the given pointer as current one, this operation should be reversed by
		/// using reset pointer with the returned integer
		Token Set(Pointer *Pointer);
		////Sets the given pointer as current one, this operation should be reversed by
		/// using reset pointer with the returned integer
		Token Set(Pointer::PointerType Type);
		////This function changes base pointer to the given one, it is best to use this function
		/// once at the startup. This function or FetchFolders function should be called before 
		/// calling show pointer function
		void ChangeBase(Pointer *Pointer);
		void ChangeBase(Pointer &Pointer) { ChangeBase(&Pointer); }
		////Removes a given stack no from pointer display list. The displayed pointer is always
		/// the one at the top. But any pointer can be removed from the list without the requirement
		/// of being on top. 
		void Reset(Token StackNo);
		////Shows current pointer, ChangeBasePointer or FetchPointers function should be called before
		/// calling this function
		void Show();
		////Hides current pointer
		void Hide();

		PointerCollection() : PointerLayer(NULL), BasePointer(NULL), PointerVisible(false), OSPointerVisible(true) { }

	protected:
		utils::SortedCollection<Pointer> ActivePointers;
		graphics::Basic2DLayer *PointerLayer;
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
