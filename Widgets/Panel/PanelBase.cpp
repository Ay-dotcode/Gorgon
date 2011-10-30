#include "PanelBase.h"
#include "..\..\Utils\Size2D.h"
#include "..\..\Resource\BitmapFontResource.h"

using namespace gge::utils;
using namespace gge::resource;
using namespace gge::graphics;
using namespace gge::resource;
using namespace std;


namespace gge { namespace widgets {
	namespace panel {




		void Base::draw() {
			if(!BaseLayer)
				return;


			BaseLayer->Clear();
			overlayer.Clear();


			prepare();


			RGBint c=0xffffffff;
			c.a=Alpha;
			BaseLayer->Ambient=c;

			adjustlayers();


			Bounds outer=BaseLayer->BoundingBox;
			Bounds inner=Bounds(0,0,outer.Width(), outer.Height());
			inner=inner-innermargins-padding;

			if(outerborder)
				inner=inner-(outerborder->BorderWidth+outerborder->Padding);


			if(outerborder) {
				RGBint c=0xffffffff;
				c.a=BGAlpha;
				BaseLayer->SetCurrentColor(c);

				outerborder->DrawIn(BaseLayer, 
					outerborder->Margins.Left, outerborder->Margins.Top,
					outer.Width()-outerborder->Margins.TotalX(),
					outer.Height()-outerborder->Margins.TotalY()
				);

				BaseLayer->SetCurrentColor(0xffffffff);
			}

			if(overlay) {
				overlay->DrawIn(overlayer, 
					overlay->Margins.Left, overlay->Margins.Top,
					outer.Width()-overlay->Margins.TotalX(),
					outer.Height()-overlay->Margins.TotalY()
				);
			}

			
		}

		void Base::style_anim_finished() {
			if(style.to==Blueprint::Style_None && style_anim_loop)
				style_anim.ResetProgress();

			if(style.to==Blueprint::Style_None && next_style==Blueprint::Style_None)
				return;

			style_anim.Pause();

			if(style.to!=Blueprint::Style_None) {
				style.from=style.to;
				style.to=Blueprint::Style_None;
			}

			if(next_style!=Blueprint::Style_None) {
				Blueprint::StyleType v=next_style, t=Blueprint::Style_None;

				if(style.from!=widgets::Blueprint::Normal) {
					if(next_style==widgets::Blueprint::Disabled) {
						v=widgets::Blueprint::Normal;
						t=next_style;
					}
				}
				else if(style.from==widgets::Blueprint::Disabled) {
					if(next_style!=widgets::Blueprint::Normal) {
						v=widgets::Blueprint::Normal;
						t=next_style;
					}
				}

				next_style=t;
				setstyle(v);
			}

			Draw();
		}

		void Base::setstyle(Blueprint::StyleType type) {
			if(!bp) {
				style.from=type;
				return;
			}

			if(style.from!=type || style.to!=Blueprint::Style_None) {
				if(style.from!=type && style.to!=type && style.to!=Blueprint::Style_None) {
					next_style=type;
					return;
				}
				if(style.from==widgets::Blueprint::Disabled && type!=widgets::Blueprint::Normal || style.from!=widgets::Blueprint::Normal && type==widgets::Blueprint::Disabled) {
					next_style=type;
					type=widgets::Blueprint::Normal;
				}
				if(style.from!=widgets::Blueprint::Active && type==widgets::Blueprint::Moving || style.from==widgets::Blueprint::Moving && type!=widgets::Blueprint::Active) {
					next_style=type;
					type=widgets::Blueprint::Active;
				}
				Blueprint::AnimationInfo info;
				if(style.from==type)
					info=bp->HasStyleAnimation(Blueprint::StyleMode(style.to, type));
				else
					info=bp->HasStyleAnimation(Blueprint::StyleMode(style.from, type));

				if(info) {
					if(style.from==type) {
						if(info.direction==style_anim.GetSpeed()) {
							next_style=type;
							return;
						}
						else
							style.from=style.to;
					}
					else if(style.to==type) {
						next_style=Blueprint::Style_None;

						return;
					}

					if(info.duration==-2) {
						info.duration=-1;
						style_anim_loop=true;
					}
					else
						style_anim_loop=false;

					if(info.direction==Blueprint::Forward)
						style_anim.SetPauseAt(info.duration);
					else
						style_anim.SetPauseAt(0);

					style_anim.SetSpeed((float)info.direction);
					style_anim.ClearFinished();
					style_anim.Resume();

					style.to=type;
				}
				else {
					style.from=type;
					style.to=Blueprint::Style_None;

					if(next_style!=Blueprint::Style_None) {
						Blueprint::StyleType v=next_style, t=Blueprint::Style_None;

						if(style.from!=widgets::Blueprint::Normal) {
							if(next_style==widgets::Blueprint::Disabled) {
								v=widgets::Blueprint::Normal;
								t=next_style;
							}
						}
						else if(style.from==widgets::Blueprint::Disabled) {
							if(next_style!=widgets::Blueprint::Normal) {
								v=widgets::Blueprint::Normal;
								t=next_style;
							}
						}

						next_style=t;
						setstyle(v);
					}
				}
			}

			Draw();
		}

		void Base::adjustlayers() {

			prepare();

			if(!BaseLayer)
				return;

			Bounds outer=BaseLayer->BoundingBox;
			Bounds inner=Bounds(0,0,outer.Width(), outer.Height());
			inner=inner-innermargins-padding-scrollmargins;

			if(outerborder)
				inner=inner-(outerborder->BorderWidth+outerborder->Padding);

			overlayer.BoundingBox=Bounds(0,0,outer.Width(), outer.Height());
			//if(innerlayer.BoundingBox!=inner)
			//	Draw();

			innerlayer.BoundingBox=inner;

			Size size=Size(inner.Width(), 0);
			for(auto i=Widgets.First();i.isValid();i.Next()) {
				int y=i->GetBounds().BottomRight().y;
				if(size.Height<y)
					size.Height=y;
			}

			//if(scrollinglayer.BoundingBox!=Bounds(Point(0,0), size))
			//	Draw();
			scrollinglayer.BoundingBox=Bounds(Point(0,0), size);

		//TODO scrolling border margins
			background.BoundingBox=scrollinglayer.BoundingBox;
			widgetlayer.BoundingBox=scrollinglayer.BoundingBox;
			extenderlayer.BoundingBox=scrollinglayer.BoundingBox;

			//allows extra 1/2 scroll
			if(scroll.y<-(size.Height-inner.Height()) && (size.Height-inner.Height())>0) {
				vscrollto(size.Height-inner.Height());
			}

			scrollinglayer.Move(scroll);
		}

		void Base::prepare() {
			if(!unprepared)
				return;


			outerborder=NULL;
			overlay=NULL;
			Alpha=255;
			BGAlpha=255;

			BorderDataResource		*bprovider;
			//animation::RectangularGraphic2DSequenceProvider *iprovider;
			bool transition;

			if(bp) {
				bprovider=bp->GetOuterBorder(style, transition);
				if(bprovider) {
					if(BorderCache[bprovider])
						outerborder=BorderCache[bprovider];
					else {
						outerborder=&bprovider->CreateResizableObject();
						BorderCache[bprovider]=outerborder;
					}

					outerborder->SetController(getanimation(transition));
				}

				bprovider=bp->GetOverlay(style, transition);
				if(bprovider) {
					if(BorderCache[bprovider])
						overlay=BorderCache[bprovider];
					else {
						overlay=&bprovider->CreateResizableObject();
						BorderCache[bprovider]=overlay;
					}

					overlay->SetController(getanimation(transition));
				}

				Alpha=bp->GetOpacity(style);
				BGAlpha=bp->GetBGOpacity(style);
			}

			unprepared=false;
		}

		void Base::focus_changed(WidgetBase *newwidget) {
			using namespace gge::utils;
			if(newwidget) {
				if(!IsFocused())
					Focus();


				if(vscroll.allow) {
					//Y scroll
					if(newwidget->GetTop()+scroll.y<0) {
						vscrollto(newwidget->GetTop());
						adjustlayers();
					}
					else if(newwidget->GetTop()+newwidget->GetHeight()+scroll.y>innerlayer.GetHeight()) {
						vscrollto(newwidget->GetTop()+newwidget->GetHeight()-innerlayer.GetHeight());
						adjustlayers();
					}
				}
			}
			else if(!allownofocus) {
				if(IsFocused())
					RemoveFocus();
			}
		}

		bool Base::MouseEvent(input::mouse::Event::Type event, utils::Point location, int amount) {
			if(event==input::mouse::Event::Left_Down) {
				Focus();
			}
			if(allowmove) {
				if(event==input::mouse::Event::Left_Down) {
					move_mlocation=location;
					move_mdown=true;

					return true;
				}

				if(event==input::mouse::Event::Move) {
					if(move_mdown && !move_ongoing) {
						if(move_mlocation.Distance(location)>3) {
							move_ongoing=true;
							setstyle(widgets::Blueprint::Moving);
							if(!move_pointer)
								move_pointer=Pointers.Set(Pointer::Drag);
						}
					}

					if(move_ongoing) {
						MoveBy(location-move_mlocation);
					}

					return true;
				}
				if(event==input::mouse::Event::Left_Up) {
					move_ongoing=false;
					move_mdown=false;
					setstyle(widgets::Blueprint::Active);
					Pointers.Reset(move_pointer);
					move_pointer=PointerCollection::NullToken;

					return true;
				}
			}

			if(vscroll.allow && event==input::mouse::Event::VScroll) {
				vscrollby(-amount);
			}

			return WidgetBase::MouseEvent(event, location, amount);
		}

		void Base::adjustscrolls() {
			adjustlayers();

			int yscrollrange=scrollinglayer.BoundingBox.Height()-innerlayer.BoundingBox.Height();
			if(yscrollrange<0)
				yscrollrange=0;

			int pmax=vscroll.bar.Max;
			vscroll.bar.Max=yscrollrange;


			if(vscroll.show && (!vscroll.autohide || vscroll.bar.Max!=0)) {
				scrollmargins.Right=vscroll.bar.GetWidth()+padding.Right;
				//if(innerborder)
				vscroll.bar.SetHeight(innerlayer.GetHeight());
				vscroll.bar.Show();
			}
			else {
				scrollmargins=Margins(0);
				vscroll.bar.Hide();
			}


			adjustlayers();
			Draw();

			vscroll.bar.Move(innerlayer.BoundingBox.Right+padding.Right, innerlayer.BoundingBox.Top);
		}

	}
}}

