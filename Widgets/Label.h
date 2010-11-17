#pragma once

#include "IWidgetObject.h"
#include "../Resource/BitmapFontResource.h"
#include "../Engine/GraphicLayers.h"

using namespace gge;
using namespace gre;

namespace gorgonwidgets {
	class Label : public IWidgetObject {
	public:
		BitmapFontResource *Font;
		RGBint Color;
		ShadowParams Shadow;
		Colorizable2DLayer textlayer;
		TextAlignment Align;
		bool Ghost;

		Label(IWidgetContainer &container, BitmapFontResource *font, string text, int X=0, int Y=0, bool autosize=true, int W=100, 
		TextAlignment align=TEXTALIGN_LEFT, RGBint color=0xff000000, ShadowParams shadow=ShadowParams()) : 
			IWidgetObject(container),autosize(autosize),
			Font(font), Color(color), Shadow(shadow),
			Align(align), textlayer(0,0, INT_MAX,INT_MAX), Ghost(true)
		{
			layer.Add(textlayer,0);
			//layer->EnableClipping=true;

			if(!autosize)
				IWidgetObject::Resize(W, font->FontHeight());

			SetText(text);
			this->Move(X, Y);

			PointerType=Pointer::Arrow;
			nofocus=true;
		}

		Label(BitmapFontResource *font, string text, int X=0, int Y=0, bool autosize=true, int W=100, 
		TextAlignment align=TEXTALIGN_LEFT, RGBint color=0xff000000, ShadowParams shadow=ShadowParams()) : 
			IWidgetObject(),autosize(autosize),
			Font(font), Color(color), Shadow(shadow),
			Align(align), textlayer(0,0, INT_MAX,INT_MAX), Ghost(true)
		{
			layer.Add(textlayer,0);
			//layer->EnableClipping=true;

			if(!autosize)
				IWidgetObject::Resize(W, font->FontHeight());

			SetText(text);
			this->Move(X, Y);

			PointerType=Pointer::Arrow;
			nofocus=true;
		}
		
		virtual void Draw() {
			textlayer.Clear();
			Font->Print(textlayer, 0, 0, width, text, Color, Align, Shadow);
		}

		virtual void SetBluePrint(IWidgetBluePrint *BP) { }
	
		virtual bool keyb_event(KeyboardEventType event,int keycode,KeyboardModifier modifier) {
			return false;
		}

		virtual bool mouse_event(MouseEventType event,int x,int y) {
			if(event==MOUSE_EVENT_LCLICK) {
				ClickEvent();
				return true;
			}

			
			return !Ghost;
		}

		virtual void on_focus_event(bool state,IWidgetObject *related) { 
			
		}
		virtual void Resize(int W, int H) {
			if(!autosize)
				IWidgetObject::Resize(W, 0);
		}
		virtual void SetText(string text) {
			if(autosize) {
				if(Align==TEXTALIGN_CENTER) {
					int l=(Font->TextWidth(text)-Font->TextWidth(this->text))/2;

					if(l) this->Move(x-l, y);
				}
				else if(Align==TEXTALIGN_RIGHT) {
					int l=Font->TextWidth(text)-Font->TextWidth(this->text);

					if(l) this->Move(x-l, y);
				}

				IWidgetObject::SetText(text);
				autoresize();
			} else {
				EPrintData ep[1];
				ep[0].Type=EMT_PositionDetect;
				ep[0].CharPosition=text.length();

				Font->Print_Test(0,0, width, text, ep,1, Align);
				IWidgetObject::Resize(width, ep[0].Out.position.y);

				IWidgetObject::SetText(text);
			}

		}



		void setAutosize(bool autosize) {
			this->autosize=autosize;
			if(autosize) autoresize();
		}

		bool getAutosize() {
			return autosize;
		}




	protected:
		bool autosize;
		void autoresize() {
			IWidgetObject::Resize(Font->TextWidth(text), Font->FontHeight());
		}
	};
}
