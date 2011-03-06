#pragma once 

#include "../Utils/LinkedList.h"
//#include "../Engine/GGEMain.h"
#include "../Engine/Input.h"
#include "../Engine/GraphicLayers.h"
#include "IWidgetContainer.h"

using namespace gge;

namespace gorgonwidgets {
	class IWidgetObject;

	class IWidgetContainer {
		friend class IWidgetObject;
	public:
		virtual IWidgetObject *AddWidget(IWidgetObject *Widget);
		virtual IWidgetObject *RemoveWidget(IWidgetObject *Widget);
		virtual IWidgetObject &AddWidget(IWidgetObject &Widget);
		virtual IWidgetObject &RemoveWidget(IWidgetObject &Widget);

		virtual IWidgetObject *AddDialog(IWidgetObject *Widget);
		virtual IWidgetObject *RemoveDialog(IWidgetObject *Widget);
		virtual IWidgetObject &AddDialog(IWidgetObject &Widget);
		virtual IWidgetObject &RemoveDialog(IWidgetObject &Widget);

		IWidgetContainer &operator += (IWidgetObject &Widget);
		IWidgetContainer &operator -= (IWidgetObject &Widget);

		IWidgetContainer(LayerBase &Parent, int X, int Y, int W, int H, int Order=0);
		IWidgetContainer(int X=0, int Y=0, int W=100, int H=100, int Order=0);

		virtual void				Show() { BaseLayer.isVisible=true; }
		virtual void				Hide();
		virtual void				Enable() { isenabled=true; }
		virtual void				Disable() { isenabled=false; }
		virtual bool				isEnabled() { return isenabled; }
		virtual bool				isVisible() { return BaseLayer.isVisible; }
		virtual void				Draw()=0;
		virtual int					X() { return BaseLayer.X; }
		virtual int					Y() { return BaseLayer.Y; }
		virtual int					Width() { return BaseLayer.W; }
		virtual int					Height() { return BaseLayer.H; }
		virtual void				SetFocus(IWidgetObject *Object);
		virtual void				SetFocus(IWidgetObject &Object);
		virtual void				Activate();
		virtual void				Deactivate();
		virtual IWidgetObject		&getFocussed() { return *Focussed; }
		virtual IWidgetObject		&getDefault() { return *Default; }
		virtual IWidgetObject		&getCancel() { return *Cancel; }
		virtual void				FocusNext();
		virtual void				FocusPrevious();
		virtual void				Move(int X,int Y);
		virtual void				Resize(int Width,int Height);
		virtual void				SetDefault(IWidgetObject *Object);
		virtual void				SetCancel(IWidgetObject *Object);
		virtual bool				isActive() { return isactive; }
		virtual int					getOrder() { return BaseLayer.getOrder(); }
		virtual void				ZOrder(int Order=-1) { BaseLayer.setOrder(Order); }

		virtual void				SetLayer(LayerBase *layer) { 
			int Order=getOrder();
			if(BaseLayer.parent) BaseLayer.parent->Remove(BaseLayer);
			layer->Add(BaseLayer, Order);
		}
		virtual void				SetLayer(LayerBase &layer) { SetLayer(&layer); }


		virtual Basic2DLayer		&GetBackgroundLayer() { return BackgroundLayer; }
		virtual void				RedrawBackground() { BackgroundLayer.Clear(); BackgroundRedrawEvent(); }

		virtual bool keyboard_event(KeyboardEventType event,int keycode,KeyboardModifier::Type modifier);

		int Order;

		EventChain<IWidgetContainer, empty_event_params> BackgroundRedrawEvent;

		WidgetLayer &getBaseLayer() { return BaseLayer; }
		WidgetLayer &getObjectLayer() { return ObjectLayer; }
		Basic2DLayer &getBackgroundLayer() { return BackgroundLayer; }

	protected:

		virtual bool keyboard_event_subitems(KeyboardEventType event,int keycode,KeyboardModifier::Type modifier);
		virtual bool keyboard_event_actions(KeyboardEventType event,int keycode,KeyboardModifier::Type modifier);

		WidgetLayer  BaseLayer;
		WidgetLayer  ExtenderLayer;
		WidgetLayer  ObjectLayer;
		Basic2DLayer BackgroundLayer;

		LinkedList<IWidgetObject> Subobjects;
		IWidgetObject* Focussed;
		IWidgetObject* Default;
		IWidgetObject* Cancel;
		bool isactive;
		bool isenabled;
		bool allowtabswitch;
		bool allowdefaultactions;
	};
}