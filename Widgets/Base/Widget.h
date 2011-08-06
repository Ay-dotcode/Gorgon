#pragma once


#include "Blueprint.h"
#include "..\..\Utils\Point2D.h"
#include "..\..\Utils\Size2D.h"
#include <limits>
#include "../../Engine/Pointer.h"
#include "../Basic/WidgetLayer.h"


namespace gge { namespace widgets {

	class ContainerBase;

	class WidgetBase {
		friend class ContainerBase;
	public:


		//Every widget should be given their own blueprints
		// otherwise an invalid cast will occur
		virtual void SetBlueprint(Blueprint &bp)=0;


		bool IsVisible();
		void Show();
		void Hide();
		void ToggleVisibility();
		void SetVisibility(bool visible);


		bool IsEnabled();
		bool IsDisabled();
		void Enable();
		void Disable();
		void ToggleEnabled();
		void SetEnabled(bool enabled);


		utils::Size GetSize();
		int GetWidth();
		int GetHeight();
		void Resize(int W, int H);
		void Resize(utils::Size Size);
		void SetWidth(int W);
		void SetHeight(int H);


		utils::Point GetLocation();
		int GetX();
		int GetY();
		int GetLeft();
		int GetTop();
		void Move(int X, int Y);
		void Move(utils::Point location);
		void SetX(int X);
		void SetY(int Y);
		void SetLeft(int X);
		void SetTop(int Y);


		utils::Bounds    GetBounds();
		utils::Rectangle GetRectangle();
		void SetBounds(utils::Bounds b);
		void SetRectangle(utils::Rectangle r);


		bool IsFocussed();
		bool Focus();
		bool SetFocus(bool focus);
		bool RemoveFocus(); //Should call Container->RemoveFocus
		void ForceRemoveFocus();


		int  GetZOrder();
		void SetZOrder();
		void ToTop();
		void ToBottom();

		int  GetFocusOrder();
		void SetFocusOrder();
		void FocusOrderToTop();
		void FocusOrderToBottom();


		ContainerBase *GetContainer();
		bool HasContainer();
		void SetContainer(ContainerBase &container);
		void SetContainer(ContainerBase *container);
		void Detach();

		WidgetLayer *GetBaseLayer();


		Pointer::PointerTypes GetPointer();
		void SetPointer(Pointer::PointerTypes pointer);
		void ResetPointer();


		virtual bool KeyboardEvent(input::keyboard::Event::Type event, int Key);

		virtual bool MouseEvent(input::mouse::Event::Type event, utils::Point location, int amount);



		utils::EventChain<WidgetBase> GotFocus;
		utils::EventChain<WidgetBase> LostFocus;


	protected:

		//TO BE CALLED BY CONTAINER
		virtual bool loosefocus(bool force) { return true; }
		virtual bool located(ContainerBase *container, int Order);
		bool detach(ContainerBase *container) {
			Container=NULL;
			delete BaseLayer;
			BaseLayer=NULL;
		}
		
		
		//FRIEND HELPERS
		bool call_container_setfocus();


		WidgetLayer   *BaseLayer;
		ContainerBase *Container;

		bool isvisible;
		bool isenabled;
	
		utils::Point location;
		utils::Size	 size;
	};

}}
