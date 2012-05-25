#include "PanelBase.h"
#include "../../Utils/Size2D.h"
#include "../../Resource/BitmapFont.h"

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
			innerlayer.Clear();
			overlayer.Clear();

			if(!display)
				return;


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
				Margins tm;
				if(outerborder) {
					tm=outerborder->BorderWidth-overlay->BorderWidth;
				}
				overlay->DrawIn(overlayer, 
					overlay->Margins.Left+tm.Left, overlay->Margins.Top+tm.Top,
					outer.Width()-overlay->Margins.TotalX()-tm.TotalX(),
					outer.Height()-overlay->Margins.TotalY()-tm.TotalY()
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
				if( (style.from==widgets::Blueprint::Disabled && type!=widgets::Blueprint::Normal) || (style.from!=widgets::Blueprint::Normal && type==widgets::Blueprint::Disabled) ) {
					next_style=type;
					type=widgets::Blueprint::Normal;
				}
				if( (style.from!=widgets::Blueprint::Active && type==widgets::Blueprint::Moving) || (style.from==widgets::Blueprint::Moving && type!=widgets::Blueprint::Active) ) {
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
			if(!BaseLayer)
				return;

			if(!bp)
				return;

			BaseLayer->Resize(WidgetBase::size.Width ? WidgetBase::size.Width : bp->DefaultSize.Width, 
				WidgetBase::size.Height ? WidgetBase::size.Height : bp->DefaultSize.Height);
			controls.Resize(BaseLayer->GetSize());

			prepare();


			Bounds outer=BaseLayer->BoundingBox;
			Bounds inner=Bounds(0,0,outer.Width(), outer.Height());
			inner=inner-innermargins-padding-scrollmargins-controlmargins;

			if(outerborder)
				inner=inner-(outerborder->BorderWidth+outerborder->Padding);

			if(innerborder) {
				inner=inner-innerborder->Margins;
			}

			overlayer.BoundingBox=Bounds(0,0,outer.Width(), outer.Height());
			//if(innerlayer.BoundingBox!=inner)
			//	Draw();

			innerlayer.BoundingBox=inner;

			Size size=Size(inner.Width(), 0);
			for(auto i=Widgets.First();i.IsValid();i.Next()) {
				int y=i->GetBounds().BottomRight().y;
				if(size.Height<y)
					size.Height=y;
			}

			//if(scrollinglayer.BoundingBox!=Bounds(Point(0,0), size))
			//	Draw();
			scrollinglayer.BoundingBox=Bounds(Point(0,0), size);

			//TODO scrolling border margins
			if(scrollingborder) {
				scrollinglayer.BoundingBox=scrollinglayer.BoundingBox-scrollingborder->Margins;
			}
			background.BoundingBox=scrollinglayer.BoundingBox;
			widgetlayer.BoundingBox=scrollinglayer.BoundingBox;
			extenderlayer.BoundingBox.Top=scrollinglayer.BoundingBox.Top;
			extenderlayer.BoundingBox.Left=scrollinglayer.BoundingBox.Left;

			//allows extra 1/5 scroll
			if(scroll.y<-(size.Height-inner.Height()+inner.Height()/5) && (size.Height-inner.Height())>0) {
				vscrollto(size.Height-inner.Height()+inner.Height()/5);
			}

			scrollinglayer.Move(scroll);

			extenderlayer.Move(innerlayer.BoundingBox.TopLeft()+scrollinglayer.BoundingBox.TopLeft());
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

				bprovider=bp->GetInnerBorder(style, transition);
				if(bprovider) {
					if(BorderCache[bprovider])
						innerborder=BorderCache[bprovider];
					else {
						innerborder=&bprovider->CreateResizableObject();
						BorderCache[bprovider]=innerborder;
					}

					innerborder->SetController(getanimation(transition));
				}

				bprovider=bp->GetScrollingBorder(style, transition);
				if(bprovider) {
					if(BorderCache[bprovider])
						scrollingborder=BorderCache[bprovider];
					else {
						scrollingborder=&bprovider->CreateResizableObject();
						BorderCache[bprovider]=scrollingborder;
					}

					scrollingborder->SetController(getanimation(transition));
				}

				bprovider=bp->GetTitleBorder(style, transition);
				if(bprovider) {
					if(BorderCache[bprovider])
						titleborder=BorderCache[bprovider];
					else {
						titleborder=&bprovider->CreateResizableObject();
						BorderCache[bprovider]=titleborder;
					}

					titleborder->SetController(getanimation(transition));
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

		bool Base::MouseHandler(input::mouse::Event::Type event, utils::Point location, int amount) {

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

			if(IsEnabled() && vscroll.allow && event==input::mouse::Event::VScroll) {
				vscrollby(-amount);

				return true;
			}

			return WidgetBase::MouseHandler(event, location, amount);
		}

		void Base::adjustscrolls() {
			adjustlayers();

			int yscrollrange=scrollinglayer.BoundingBox.Height()-innerlayer.BoundingBox.Height();
			if(yscrollrange<0)
				yscrollrange=0;
			else
				yscrollrange+=innerlayer.BoundingBox.Height()/5;

			vscroll.bar.Max=yscrollrange;


			if(vscroll.show && (!vscroll.autohide || vscroll.bar.Max!=0)) {
				scrollmargins.Right=vscroll.bar.GetWidth()+padding.Right;
				//if(innerborder)
				vscroll.bar.SetHeight(innerlayer.GetHeight());
				if(!vscroll.bar.IsVisible()) {
					vscroll.bar.Show();
					Reorganize();
				}
			}
			else {
				scrollmargins=Margins(0);
				if(vscroll.bar.IsVisible()) {
					vscroll.bar.Hide();
					Reorganize();
				}
			}


			adjustlayers();
			Draw();

			vscroll.bar.Move(innerlayer.BoundingBox.Right+padding.Right, innerlayer.BoundingBox.Top);
		}

		utils::Point Base::AbsoluteLocation() {
			if(Container) {
				return Container->AbsoluteLocation()+location;
			}
			else
				return location;
		}

		void Base::adjustcontrols() {
			controlmargins.Top=0;
			controlmargins.Bottom=0;
			int tbuttonw=0, tbuttonx=0;

			utils::Size size;
			if(!bp)
				size=WidgetBase::size;
			else
				size=utils::Size(WidgetBase::size.Width ? WidgetBase::size.Width : bp->DefaultSize.Width,
					WidgetBase::size.Height ? WidgetBase::size.Height : bp->DefaultSize.Height)	;

			prepare();

			Margins bordermargins(0);
			if(outerborder) {
				bordermargins=outerborder->BorderWidth+outerborder->Padding;
			}
			if(innerborder) {
				bordermargins+=innerborder->Margins;
			}

			if(titlebuttons.GetCount()>0/* && showtitlebtn*/) {
				int height=titlebuttons[0].GetHeight();

				Alignment::Type align(Alignment::Middle_Right);
				Margins margins(0);

				if(this->bp && this->bp->TitleButtonPlace) {
					align=this->bp->TitleButtonPlace->Align;
					height=this->bp->TitleButtonPlace->Minimum.Height;
					margins=this->bp->TitleButtonPlace->Margins;
				}

				if(TextAlignment::GetHorizontal(align)==TextAlignment::Left) {
					int x=margins.Left;
					for(auto i=titlebuttons.Last();i.IsValid();i.Previous()) {
						if(i->IsVisible()) {
							int y=Alignment::CalculateLocation(align, Bounds(0,margins.Top, 0,height+margins.TotalY()), Size(0,i->GetHeight())).y;

							i->Move(x,y);
							x+=i->GetWidth();
						}
					}

					tbuttonx=x;
					tbuttonw=x;
				}
				else {
					int x=size.Width-margins.Right;
					for(auto i=titlebuttons.Last();i.IsValid();i.Previous()) {
						if(i->IsVisible()) {
							int y=Alignment::CalculateLocation(align, Bounds(0,margins.Top, 0,height+margins.TotalY()), Size(0,i->GetHeight())).y;

							x-=i->GetWidth();
							i->Move(x,y);
						}
					}

					tbuttonw=size.Width-x;
				}

				controlmargins.Top=max(height+margins.TotalY()-bordermargins.Top,0);
			}

			if(dialogbuttons.GetCount()>0/* && showdialogbtn*/) {
				int height=dialogbuttons[0].GetHeight();

				Alignment::Type align(Alignment::Middle_Right);
				Margins margins(0);

				if(this->bp && this->bp->DialogButtonPlace) {
					align=this->bp->DialogButtonPlace->Align;
					height=this->bp->DialogButtonPlace->Minimum.Height;
					margins=this->bp->DialogButtonPlace->Margins;
				}

				int ymod=0;
				if(TextAlignment::GetHorizontal(align)==TextAlignment::Left) {
					int x=margins.Left;
					int cw=0;
					for(auto i=dialogbuttons.Last();i.IsValid();i.Previous()) {
						if(i->IsVisible()) {
							int y=Alignment::CalculateLocation(align, Bounds(0,controls.BaseLayer.BoundingBox.Bottom-height-margins.TotalY(), 0,controls.BaseLayer.BoundingBox.Bottom-margins.Bottom), Size(0,i->GetHeight())).y;

							if(cw+i->GetWidth()>controls	.GetWidth()-margins.TotalX() && cw>0) {
								ymod+=height;
								cw=0;
								x=margins.Left;
							}

							i->Move(x,y-margins.Bottom-ymod);
							x+=i->GetWidth();
							cw+=i->GetWidth();
						}
					}
				}
				else {
					int x=size.Width-margins.Right;
					int cw=0;
					for(auto i=dialogbuttons.Last();i.IsValid();i.Previous()) {
						if(i->IsVisible()) {
							int y=Alignment::CalculateLocation(align, Bounds(0,controls.BaseLayer.BoundingBox.Bottom-(height+margins.TotalY()), 0,controls.BaseLayer.BoundingBox.Bottom-margins.Bottom), Size(0,i->GetHeight())).y;

							if(cw+i->GetWidth()>controls	.BaseLayer.BoundingBox.Width()-margins.TotalX() && cw>0) {
								ymod+=height;
								cw=0;
								x=size.Width-margins.Right;
							}

							x-=i->GetWidth();
							i->Move(x,y-ymod);
							cw+=i->GetWidth();
						}
					}
				}

				controlmargins.Bottom=max(height+ymod+margins.TotalY()-bordermargins.Bottom,0);
			}

			if(showtitle) {
				controlmargins.Top=max(controlmargins.Top,title.GetHeight()-bordermargins.Top);
				title.SetWidth(size.Width-tbuttonw);
				title.SetX(tbuttonx);
			}


			adjustscrolls();
		}

		void Base::setblueprint(const widgets::Blueprint &bp) {
			this->bp=static_cast<const Blueprint*>(&bp);

			clearcaches();

			for(auto i=titlebuttons.begin();i!=titlebuttons.end();++i)
				if(dynamic_cast<Button*>(&*i))
					i->SetBlueprint(this->bp->TitleButton);

			for(auto i=dialogbuttons.begin();i!=dialogbuttons.end();++i)
				if(dynamic_cast<Button*>(&*i))
					i->SetBlueprint(this->bp->DialogButton);

			if(this->bp)
				if(this->bp->Scroller)
					vscroll.bar.SetBlueprint(*this->bp->Scroller);

			if(this->bp) {
				this->pointer=bp.Pointer;
				title.SetBlueprint(this->bp->TitleLabel);
			}

			unprepared=true;
			adjustcontrols();
			Draw();
		}

		Base::Base() : innermargins(0),
			allownofocus(false), allowmove(false), allowresize(false),
			controls(*this),dialogcontrols(*this),
			bp(NULL), next_style(widgets::Blueprint::Style_None),
			move_mdown(false), move_ongoing(false), padding(5),
			move_pointer(PointerCollection::NullToken), scroll(0,0),
			vscroll(true), scrollmargins(0), controlmargins(0), outerborder(NULL),
			scrollingborder(NULL), innerborder(NULL), showtitle(false), display(true),
			blueprintmodified(false) {
				padding=utils::Margins(WR.WidgetSpacing.x,WR.WidgetSpacing.y);

				controls.alwaysenabled=true;

				innerlayer.Add(scrollinglayer);
				scrollinglayer.Add(background, 1);
				scrollinglayer.Add(widgetlayer, 0);
				innerlayer.ClippingEnabled=true;

				style_anim.Pause();
				style_anim.Finished.Register(this, &Base::style_anim_finished);
				style_anim.Paused.Register(this, &Base::style_anim_finished);

				vscroll.bar.Hide();
				vscroll.bar.SetContainer(controls);
				vscroll.bar.AllowFocus=false;
				vscroll.bar.SmallChange=60;
				vscroll.bar.LargeChange=120;
				vscroll.bar.ChangeEvent.Register(this, &Base::vscroll_change);

				title.Hide();
				title.SetContainer(controls);
				title.Autosize=AutosizeModes::None;
				title.TextWrap=false;

				WR.LoadedEvent.Register(this, &Base::wr_loaded);
		}

		void Base::clearcaches() {
			for(auto i=BorderCache.begin();i!=BorderCache.end();++i)
				if(i->second)
					i->second->DeleteAnimation();

			BorderCache.clear();

			for(auto i=ImageCache.begin();i!=ImageCache.end();++i)
				if(i->second)
					i->second->DeleteAnimation();

			ImageCache.clear();
		}

	}
}}

