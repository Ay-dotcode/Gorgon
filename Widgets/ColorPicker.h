#pragma once

#include "Base/Widget.h"
#include "Basic/PetContainer.h"
#include "LayerWidget.h"
#include "../Resource/File.h"
#include "../Resource/Image.h"
#include <string.h>
#include "../Utils/Property.h"

namespace gge { namespace widgets {

	//currently extremely simple and a single mode of operation
	//made inextensible intentionally
	class ColorPicker : public WidgetBase {
	public:
		ColorPicker()  : controls(*this), ChangeEvent(this), color(0xff000000),
			INIT_PROPERTY(ColorPicker, Value)
		{
			display.UseHex=true;
			display.Value=0xff000000;
			display.Prefix="0x";
			display.SetContainer(controls);
			display.SetWidth(78);
			display.Move(90, (40-display.GetHeight())/2);
			display.ChangeEvent.RegisterLambda([&]{
				color=display.Value;
				tempcolor=color;
				Draw();
				ChangeEvent();
			});

			visual.SetContainer(controls);
			visual.Resize(imsize,36);
			visual.Move(2,2);
			visual.GetLayer().MouseCallback.SetLambda([&](gge::input::mouse::Event::Type event, gge::utils::Point location, int amount) -> bool {
				if(event==gge::input::mouse::Event::Move) {
					if(location.x>=64) {
						if((int)tempcolor!=(int)color) {
							tempcolor=color;
							draw();
						}
					}
					else {
						tempcolor=colorbuffer[location.x+location.y*imsize];
						tempcolor.a=color.a;
						Draw();
					}
				}
				else if(event==gge::input::mouse::Event::Left_Click) {
					if(location.x<64) {
						Byte ta=color.a;
						color=colorbuffer[location.x+location.y*imsize];
						color.a=ta;
					}
					else {
						color=colorbuffer[location.x+location.y*imsize];
					}
					display.Value.Set((int)color);
					ChangeEvent();
					Draw();
				}
				else if(event==gge::input::mouse::Event::Out) {
					tempcolor=color;
					Draw();
				}

				return !gge::input::mouse::Event::isScroll(event);
			});

			image.Resize(imsize, 36, gge::graphics::ColorMode::ARGB);
			memset(image.getdata().GetBuffer(), 0, imsize*36*4);
			colorbuffer=(graphics::RGBint*)image.getdata().GetBuffer();

			Resize(168, 40);
		}

		virtual void SetBlueprint(const gge::widgets::Blueprint &bp) {}


		virtual bool Focus() {
			if(!IsFocused()) {
				WidgetBase::Focus();

				display.Focus();
			}
			return true;
		}

		virtual void Enable() {
			controls.Enable();
			WidgetBase::Enable();
		}

		virtual void Disable() {
			controls.Disable();
			WidgetBase::Disable();
		}
		using WidgetBase::Resize;
		virtual void Resize(gge::utils::Size Size) {
			WidgetBase::Resize(Size);


			if(BaseLayer)
				BaseLayer->Resize(Size);

			controls.Resize(Size);
		}

		virtual bool KeyboardHandler(input::keyboard::Event::Type event, input::keyboard::Key Key) {
			return display.KeyboardHandler(event, Key);
		}


		utils::Property<ColorPicker, graphics::RGBint> Value;
		utils::EventChain<ColorPicker> ChangeEvent;


	protected:
		void drawpixel(int row, int col, graphics::RGBint color) {
			for(int yy=row*4;yy<row*4+4;yy++)
				for(int xx=col*4;xx<col*4+4;xx++)
					colorbuffer[yy*imsize+xx]=color;
		}

		virtual void draw() {
			auto &l=visual.GetLayer();

			l.Clear();

			for(int i=0;i<16;i++) {
				drawpixel(0, i, lch_rgb(i*(100.f/15.f), 0, 0));
			}
			for(int i=0;i<8;i++) {
				for(int h=0;h<16;h++) {
					drawpixel(i+1, h, lch_rgb((i/4)?66.f:33.f, (i%4)*25+25.f, h*(360.f/16.f)));
				}
			}

			for(int i=0;i<9;i++) {
				drawpixel(i, 20, tempcolor);
			}

			for(int i=0;i<9;i++) {
				tempcolor.a=gge::Byte(i*(255.f/8.f));
				drawpixel(i, 17, tempcolor);
				drawpixel(i, 18, tempcolor);
			}

			image.Prepare(Main, dummy);
			image.DrawIn(l);
		}

		virtual void containerenabledchanged(bool state) {
			controls.InformEnabledChange(state);
		}

		virtual bool detach(gge::widgets::ContainerBase *container) {
			controls.AttachTo(NULL);

			return true;
		}

		virtual void located(gge::widgets::ContainerBase* container, gge::utils::SortedCollection<WidgetBase>::Wrapper *w, int Order) {
			WidgetBase::located(container, w, Order);

			if(BaseLayer)
				BaseLayer->Resize(controls.GetSize());

			if(container) {
				controls.AttachTo(BaseLayer);
				containerenabledchanged(container->IsEnabled());
			}
			else
				controls.AttachTo(NULL);
		}

		virtual bool loosefocus(bool force) {
			if(force) {
				display.ForceRemoveFocus();
				return true;
			}
			else
				return display.RemoveFocus();
		}

		graphics::RGBint lch_rgb(float l, float c, float h) {
			graphics::RGBint ret;

			l = l / 100;
			float a = std::cos(  h * 0.01745329252f  ) * (c / 100);
			float b = std::sin(  h * 0.01745329252f  ) * (c / 100);

			float fy =l;
			float fx = fy + a / 5.0f;
			float fz = fy - b / 2.0f;
			float y = fy * fy * fy ;
			float x = fx * fx * fx ;
			float z = fz * fz * fz ;

			float lr = ( 3.2410f * 0.9505f) * x + (-1.5374f) * y + (-0.4986f * 1.0890f) * z;
			float lg = (-0.9692f * 0.9505f) * x + ( 1.8760f) * y + ( 0.0416f * 1.0890f) * z;
			float lb = ( 0.0556f * 0.9505f) * x + (-0.2040f) * y + ( 1.0570f * 1.0890f) * z;

			lr=gge::utils::Limit(lr, 0.f, 1.f);
			lg=gge::utils::Limit(lg, 0.f, 1.f);
			lb=gge::utils::Limit(lb, 0.f, 1.f);

			ret.a = 255;
			ret.r = Byte(255*std::pow( lr, 1.0f / 2.2f )) ;
			ret.g = Byte(255*std::pow( lg, 1.0f / 2.2f )) ;
			ret.b = Byte(255*std::pow( lb, 1.0f / 2.2f )) ;

			return ret;
		}

		void setValue(const graphics::RGBint &value) {
			if((int)value!=(int)color) {
				color=value;
				display=value;
				tempcolor=color;

				draw();
			}
		}
		graphics::RGBint getValue() const {
			return color;
		}

		PetContainer<ColorPicker> controls;
		LayerWidget visual;
		Numberbox<unsigned> display;

		gge::resource::File dummy;
		graphics::RGBint *colorbuffer;
		gge::resource::Image image;

		graphics::RGBint color;
		graphics::RGBint tempcolor;

		static const int imsize=84;
	};

} }
