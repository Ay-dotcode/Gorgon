#include "CheckboxBase.h"
#include "..\..\Utils\Size2D.h"
#include "..\..\Resource\BitmapFontResource.h"

using namespace gge::utils;
using namespace gge::resource;
using namespace gge::graphics;
using namespace gge::resource;
using namespace std;

namespace gge { namespace widgets {
	namespace checkbox {

		int Base::lineheight(Blueprint::Line *line, int &prevymargin) {
			if(line->HeightMode==Blueprint::Fixed)
				return line->Height;

			int h=0;

			BorderDataResource *bprovider=line->Border;
			BorderData *lineborder=NULL;
			if(bprovider) {
				if(BorderCache[bprovider])
					lineborder=BorderCache[bprovider];
				else {
					lineborder=&bprovider->CreateResizableObject();
					BorderCache[bprovider]=lineborder;
				}
			}


			for(int i=0;i<3;i++) {
				int nh=0;
				switch(line->GetContent(i)) {
				case Blueprint::Icon:
					if(icon && drawicon) {
						nh=icon->GetHeight();
					}
					break;

				case Blueprint::Text:
					if(font && text!="") {
						nh=font->FontHeight();
					}
					break;
				case Blueprint::Symbol:
					if(symbol && drawsymbol) {
						nh=symbol->CalculateHeight(0);
					}
					break;
				}

				h=max(nh,h);
			}

			if(lineborder) {
				h+=lineborder->BorderWidth.TotalY() + lineborder->Padding.TotalY();
				int margin=(lineborder->Margins.Top > prevymargin ? lineborder->Margins.TotalY() : lineborder->Margins.Bottom);
				h+=margin;
				prevymargin=lineborder->Margins.Bottom;
			}
			else
				prevymargin=0;


			return h;
		}

		void Base::drawline(int id, Blueprint::TransitionType transition, int y, int reqh, int h, int &prevymargin) {
			//Blueprint::TransitionType transition;

			//prepare resources
			Blueprint::Line *line=lines[id];
			BorderDataResource *bprovider=line->Border;
			BorderData *lineborder=NULL;
			if(bprovider) {
				if(BorderCache[bprovider])
					lineborder=BorderCache[bprovider];
				else {
					lineborder=&bprovider->CreateResizableObject();
					BorderCache[bprovider]=lineborder;
				}

				lineborder->SetController(getanimation(transition));
			}

			if(lineborder) {
				y+=(lineborder->Margins.Top > prevymargin ? lineborder->Margins.Top-prevymargin : 0);
				prevymargin=lineborder->Margins.Bottom;
			}
			else
				prevymargin=0;

			//get id use line height and calculate new location
			if(Alignment::isMiddle(line->Align))
				y=y+(h-reqh)/2;
			else if(Alignment::isBottom(line->Align))
				y=y+(h-reqh);

			h=reqh;

			//determine content bounds
			Bounds exterior(0,y,BaseLayer->BoundingBox.Width(),y+h);
			Bounds bounds  (0,y,innerlayer.BoundingBox.Width(),y+h);

			//exterior is not draw on inner layer which is already padded
			if(border) {
				//should effect only x,y, and width but not height since height is given calculated
				exterior=border->ContentBounds(exterior);
				exterior.SetSize(exterior.Width(), h);
			}

			if(lineborder) {
				bounds=lineborder->ContentBounds(bounds);
			}


			//determine currently needed width
			int knownw=0;
			int maximumsized=0;
			int total=0;
			int totalmargin=0;
			int pmargin=0;
			int sizes[3]={};
			for(int i=0;i<3;i++) {
				switch(line->GetContent(i)) {
				case Blueprint::Icon:
					if(icon && drawicon) {
						if(iconp) {
							sizes[i]=iconp->GetSize(icon->GetSize(), icon->GetSize()).Width;

							totalmargin+=(iconp->Margins.Left>pmargin ? iconp->Margins.TotalX()-pmargin : iconp->Margins.Right);
							pmargin=iconp->Margins.Right;

							if(iconp->SizingMode==Placeholder::MaximumAvailable)
								maximumsized++;
						}
						else
							sizes[i]=icon->GetWidth();

						knownw+=sizes[i];
						total++;
					}
					break;
				case Blueprint::Text:
					if(font && text!="") {
						int w=font->TextWidth(text);
						if(textp) {
							sizes[i]=0; //text size is not fixed and might be larger than the given area

							totalmargin+=(textp->Margins.Left>pmargin ? textp->Margins.TotalX()-pmargin : textp->Margins.Right);
							pmargin=textp->Margins.Right;

							if(textp->SizingMode==Placeholder::MaximumAvailable)
								maximumsized++;
						}
						else if(line->WidthMode==Blueprint::Auto)
							sizes[i]=textp->GetSize(Size(w,0), Size(w,0)).Width;

						knownw+=sizes[i];
						total++;
					}
					break;
				case Blueprint::Symbol:
					if(symbol && drawsymbol) {
						int w=symbol->CalculateWidth(0);
						if(symbolp) {
							if(w==0)
								w=bounds.Height()-symbolp->Margins.TotalY();
							
							sizes[i]=symbolp->GetSize(Size(w,0), Size(w,0)).Width;

							totalmargin=(symbolp->Margins.Left>pmargin ? symbolp->Margins.TotalX()-pmargin : symbolp->Margins.Right);
							pmargin=symbolp->Margins.Right;

							if(textp->SizingMode==Placeholder::MaximumAvailable)
								maximumsized++;
						}
						else {
							if(w==0)
								w=bounds.Height();

							sizes[i]=w;
						}

						knownw+=sizes[i];
						total++;
					}
					break;
				}
			}

			if(total==0) return;


			knownw+=totalmargin;
			int extra=bounds.Width()-knownw;
			int x=bounds.Left;
			if(line->WidthMode==Blueprint::Auto) {
				for(int i=0;i<3;i++) {
					if(line->GetContent(i)==Blueprint::Text && font) {
						int tw=font->TextWidth(text);
						sizes[i] += min(tw, extra);
						knownw+=min(tw, extra);
						extra-=min(tw, extra);
					}
				}

				if(Alignment::isCenter(line->Align))
					x+=extra/2;
				else if(Alignment::isRight(line->Align))
					x+=extra;

				exterior.Left=x+(exterior.Left-bounds.Left);
				exterior.Right=x+knownw+(exterior.Right-bounds.Right);
				bounds.Left=x;
				bounds.Right=x+knownw;

				extra=0;
			}

			if(extra>0 && maximumsized)
				extra /= maximumsized;
			else
				extra /= total;

			if(lineborder) {
				lineborder->DrawIn(BaseLayer, exterior);
			}


			y=bounds.Top;
			h=bounds.Height();

			pmargin=0;
			Bounds itembounds;
			Point  targetlocation;
			Alignment::Type align;

			//draw items calculating, x and new widths
			for(int i=0;i<3;i++) {
				switch(line->GetContent(i)) {
				case Blueprint::Icon:
					if(icon && drawicon) {
						if(iconp) {
							if(iconp->SizingMode==Placeholder::MaximumAvailable || maximumsized==0 || extra<0) {
								sizes[i]+=extra;
							}

							x+=(iconp->Margins.Left>pmargin ? iconp->Margins.Left-pmargin : 0);
							itembounds.Left=x;
							itembounds.Top =y;
							itembounds.SetSize(sizes[i], h);
							itembounds.Top-=iconp->Margins.Top;
							itembounds.Bottom+=iconp->Margins.Bottom;

							align=iconp->Align;
							x+=iconp->Margins.Right;
						}
						else {
							if(maximumsized==0 || extra<0) {
								sizes[i]+=extra;
							}
							itembounds=utils::Rectangle(x,y,sizes[i],h);
						}

						targetlocation=Alignment::CalculateLocation(align, itembounds, icon->GetSize());

						icon->Draw(innerlayer, targetlocation);
					}

					break;

				case Blueprint::Text:
					if(font && text!="") {
						if(textp) {
							if(textp->SizingMode==Placeholder::MaximumAvailable || maximumsized==0 || extra<0) {
								sizes[i]+=extra;
							}

							x+=(textp->Margins.Left>pmargin ? textp->Margins.Left-pmargin : 0);
							itembounds.Left=x;
							itembounds.Top =y;
							itembounds.SetSize(sizes[i], h);
							itembounds.Top-=textp->Margins.Top;
							itembounds.Bottom+=textp->Margins.Bottom;

							align=textp->Align;
							x+=textp->Margins.Right;
						}
						else {
							if(maximumsized==0 || extra<0) {
								sizes[i]+=extra;
							}
							itembounds=utils::Rectangle(x,y,sizes[i],h);
						}

						int th;
						if(textwrap) 
							th=font->TextHeight(text, itembounds.Width())-(font->FontHeight()-font->FontBaseline());
						else
							th=font->FontBaseline();

						targetlocation=Alignment::CalculateLocation(align, itembounds, Size(font->TextWidth(text),th));

						if(textwrap) {
							font->Print(innerlayer, Point(itembounds.Left,targetlocation.y),itembounds.Width(),text,TextAlignment::GetHorizontal(align));
						}
						else {
							font->Print(innerlayer, targetlocation,text);
						}
						if(underline) {
							int ch=-1;
							for(int i=0;i<(int)text.length();i++) {
								if(tolower(text[i])==underline) {
									ch=i;
									break;
								}
							}

							if(ch>-1) {
								EPrintData eprint[3];
								eprint[0].Type=EPrintData::Wrap;
								eprint[0].In.value=1;
								eprint[1].Type=EPrintData::PositionDetect;
								eprint[1].CharPosition=ch;
								eprint[2].Type=EPrintData::PositionDetect;
								eprint[2].CharPosition=ch+1;

								if(textwrap) {
									font->Print_Test(Point(itembounds.Left,targetlocation.y),itembounds.Width(),
										text,eprint,3,TextAlignment::GetHorizontal(align));
								}
								else {
									eprint[0].In.value=0;
									font->Print_Test(targetlocation,itembounds.Width(),
										text,eprint,3,TextAlignment::Left);
								}

								if(dynamic_cast<BitmapFontResource*>(font->getRenderer())) {
									ImageResource *im=dynamic_cast<BitmapFontResource*>(font->getRenderer())->Characters['_'];
									innerlayer.SetCurrentColor(font->Color);
									im->DrawStretched(innerlayer, 
										eprint[1].Out.position.x-1,eprint[1].Out.position.y+(font->FontHeight()-font->FontBaseline())/2,
										2+eprint[2].Out.position.x-eprint[1].Out.position.x, im->GetHeight()
									);
									innerlayer.SetCurrentColor(0xffffffff);
								}
							}
						}
					}

					break;

				case Blueprint::Symbol:
					if(symbol && drawsymbol) {
						if(symbolp) {
							if(symbolp->SizingMode==Placeholder::MaximumAvailable || maximumsized==0 || extra<0) {
								sizes[i]+=extra;
							}

							x+=(symbolp->Margins.Left>pmargin ? symbolp->Margins.Left-pmargin : 0);
							itembounds.Left=x;
							itembounds.Top =y;
							itembounds.SetSize(sizes[i], h);
							itembounds.Top-=symbolp->Margins.Top;
							itembounds.Bottom+=symbolp->Margins.Bottom;

							align=symbolp->Align;
							x+=symbolp->Margins.Right;
						}
						else {
							if(maximumsized==0 || extra<0) {
								sizes[i]+=extra;
							}
							itembounds=utils::Rectangle(x,y,sizes[i],h);
						}

						int sh=symbol->CalculateHeight(0);
						if(sh==0) { 
							if(symbolp)
								sh=h-symbolp->Margins.TotalY();
							else 
								sh=h;
						}
						int sw=symbol->CalculateWidth(0);
						if(sw==0)
							sw=sh;

						targetlocation=Alignment::CalculateLocation(align, itembounds, Size(sw, sh));

						symbol->DrawIn(innerlayer, targetlocation, Size(sw, sh));
					}

					break;

				}

				x+=sizes[i];
			}
		}

		void Base::draw() {
			if(!BaseLayer)
				return;

			BaseLayer->Clear();
			innerlayer.Clear();

			if(!bp)
				return;

			Blueprint::Group **groups=new Blueprint::Group *[5];

			bp->GetAlternatives(groups, focus, state);

			//prepare resources
			Blueprint::TransitionType transition;

			symbolp=bp->GetSymbolPlaceholder(groups, style, transition);
			textp=bp->GetTextPlaceholder(groups, style, transition);
			iconp=bp->GetIconPlaceholder(groups, style, transition);


			ResizableObjectProvider *provider;
			BorderDataResource		*bprovider;
			symbol=NULL;
			border=NULL;
			overlay=NULL;

			font=bp->GetFont(groups, style, transition);
			provider=bp->GetSymbol(groups, style, transition);
			if(provider) {
				if(ImageCache[provider])
					symbol=ImageCache[provider];
				else {
					symbol=&provider->CreateResizableObject();
					ImageCache[provider]=symbol;
				}

				symbol->SetController(getanimation(transition));
			}
			bprovider=bp->GetOuterBorder(groups, style, transition);
			if(bprovider) {
				if(BorderCache[bprovider])
					border=BorderCache[bprovider];
				else {
					border=&bprovider->CreateResizableObject();
					BorderCache[bprovider]=border;
				}

				border->SetController(getanimation(transition));
			}
			bprovider=bp->GetOverlay(groups, style, transition);
			if(bprovider) {
				if(BorderCache[bprovider])
					overlay=BorderCache[bprovider];
				else {
					overlay=&bprovider->CreateResizableObject();
					BorderCache[bprovider]=overlay;
				}

				overlay->SetController(getanimation(transition));
			}

			//setup lines and heights
			Blueprint::TransitionType linetransitions[4];
			int lineheights[4]={};
			int lineheights_org[4]={};
			int totalknownlineheights=0;
			int totallines=0;
			int maximumsizedlines=0;

			int prevymargin=0;
			for(int i=1;i<=3;i++) {
				lines[i]=bp->GetLine(i, groups, style, transition);
				linetransitions[i]=transition;

				if(lines[i]) {
					totallines++;
					lineheights[i]=lineheight(lines[i], prevymargin);

					if(lineheights[i]>0)
						totalknownlineheights+=lineheights[i];

					if(lines[i]->HeightMode==Blueprint::MaximumAvailable)
						maximumsizedlines++;
				}
			}

			//calculate remaining height to be distributed to maximum available sized lines.
			//if no one is specified as maximum available and there is still available size,
			//give it to the last one
			int extra;
			Bounds outer=Bounds(Point(0,0),BaseLayer->BoundingBox.GetSize());
			Bounds inner=outer;
			if(border) {
				inner=border->ContentBounds(outer);
			}
			innerlayer.BoundingBox=inner;
			extra=inner.Height()-totalknownlineheights;

			for(int i=1;i<=3;i++)
				lineheights_org[i]=lineheights[i];

			if(extra>0 && maximumsizedlines>0) {
				int distribute=extra/maximumsizedlines;
				int k=maximumsizedlines;

				for(int i=1;i<=3;i++) {
					if(lines[i] && lines[i]->HeightMode==Blueprint::MaximumAvailable) {
						lineheights[i]+=distribute;
						extra-=distribute;
						
						if(--k==0)
							lineheights[i]+=extra;
					}
				}
			}
			else if(extra<0 && totallines) {
				int distribute=extra/totallines;
				int k=totallines;

				for(int i=1;i<=3;i++) {
					if(lines[i]) {
						lineheights[i]+=distribute;
						extra-=distribute;

						if(--k==0)
							lineheights[i]+=extra;
					}
				}
			}
			else {
				for(int i=3;i>0;i--) {
					if(lines[i]) {
						lineheights[i]+=extra;
						break;
					}
				}
			}

			//draw border
			if(border) {
				border->DrawIn(BaseLayer, outer);
			}

			//draw lines
			int y=0;//inner.Top;
			for(int i=1;i<=3;i++) {
				if(lines[i]) {
					drawline(i, linetransitions[i], y, lineheights_org[i], lineheights[i], prevymargin);
					y+=lineheights[i];
				}
			}

			if(overlay) {
				overlay->DrawIn(BaseLayer, outer);
			}



			delete[] groups;
		}

		void Base::setfocus(Blueprint::FocusType type) {
			if(!bp) {
				focus.from=type;
				return;
			}

			if(focus.from!=type || focus.to!=Blueprint::FT_None) {
				Blueprint::AnimationInfo info;
				if(focus.from==type)
					info=bp->HasFocusAnimation(Blueprint::FocusMode(focus.to, type));
				else
					info=bp->HasFocusAnimation(Blueprint::FocusMode(focus.from, type));

				if(info) {
					if(focus.from==type) {
						if(info.direction==focus_anim.GetSpeed()) {
							next_focus=type;
							return;
						}
						else
							focus.from=focus.to;
					}
					else if(focus.to==type) {
						next_focus=Blueprint::FT_None;

						return;
					}
					
					if(info.duration==-2) {
						info.duration=-1;
						focus_anim_loop=true;
					}
					else if(info.duration==0) {
						focus.from=type;
						Draw();

						return;
					}
					else
						focus_anim_loop=false;

					if(info.direction==Blueprint::Forward)
						focus_anim.SetPauseAt(info.duration);
					else
						focus_anim.SetPauseAt(0);

					focus_anim.SetSpeed((float)info.direction);
					focus_anim.ClearFinished();
					focus_anim.Resume();

					focus.to=type;
				}
				else
					focus.from=type;
			}

			Draw();
		}

		void Base::setstate(int type) {
			currentstate=type;
			if(!bp) {
				state.from=type;
				return;
			}

			if(state.from!=type || state.to!=0) {
				if(state.from!=type && state.to!=type && state.to!=0) {
					next_state=type;
					return;
				}
				Blueprint::AnimationInfo info;
				if(state.from==type)
					info=bp->HasStateAnimation(Blueprint::StateMode(state.to, type));
				else
					info=bp->HasStateAnimation(Blueprint::StateMode(state.from, type));

				if(info) {
					if(state.from==type) {
						if(info.direction==state_anim.GetSpeed()) {
							next_state=type;
							return;
						}
						else
							state.from=state.to;
					}
					else if(state.to==type) {
						next_state=0;

						return;
					}

					if(info.duration==-2) {
						info.duration=-1;
						state_anim_loop=true;
					}
					else
						state_anim_loop=false;

					if(info.direction==Blueprint::Forward)
						state_anim.SetPauseAt(info.duration);
					else
						state_anim.SetPauseAt(0);

					state_anim.SetSpeed((float)info.direction);
					state_anim.ClearFinished();
					state_anim.Resume();

					state.to=type;
				}
				else {
					state.from=type;
					state.to=0;

					if(next_state!=0) {
						int v=next_style;


						next_state=0;
						setstate(v);
					}
				}
			}

			Draw();
		}

		void Base::setstyle(Blueprint::StyleType type) {
			if(!bp) {
				style.from=type;
				return;
			}

			if(style.from!=type || style.to!=Blueprint::YT_None) {
				if(style.from!=type && style.to!=type && style.to!=Blueprint::YT_None) {
					next_style=type;
					return;
				}
				if(style.from==Blueprint::Disabled && type!=Blueprint::Normal || style.from!=Blueprint::Normal && type==Blueprint::Disabled) {
					next_style=type;
					type=Blueprint::Normal;
				}
				Blueprint::AnimationInfo info;
				if(style.from==type)
					info=bp->HasStyleAnimation(focus,state,Blueprint::StyleMode(style.to, type));
				else
					info=bp->HasStyleAnimation(focus,state,Blueprint::StyleMode(style.from, type));

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
						next_style=Blueprint::YT_None;

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
					style.to=Blueprint::YT_None;

					if(next_style!=Blueprint::YT_None) {
						Blueprint::StyleType v=next_style, t=Blueprint::YT_None;

						if(style.from==Blueprint::Down && next_style==Blueprint::Normal) {
							triggerwait();

							return;
						}

						if(style.from!=Blueprint::Normal) {
							if(next_style==Blueprint::Disabled) {
								v=Blueprint::Normal;
								t=next_style;
							}
						}
						else if(style.from==Blueprint::Disabled) {
							if(next_style!=Blueprint::Normal) {
								v=Blueprint::Normal;
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

		bool Base::Focus() {
			if(!cangetfocus)
				return false;

			if(!IsEnabled())
				return false;

			if(IsFocussed())
				return true;

			WidgetBase::Focus();
			setfocus(Blueprint::Focused);

			return true;
		}
	
		bool Base::loosefocus(bool force) {
			if(!IsFocussed())
				return true;

			setfocus(Blueprint::NotFocused);

			return true;
		}

		void Base::Enable() {
			WidgetBase::Enable();
			
			if(mouseover)
				setstyle(Blueprint::Hover);
			else
				setstyle(Blueprint::Normal);
		}

		void Base::Disable() {
			WidgetBase::Disable();
			
			setstyle(Blueprint::Disabled);
		}

		utils::Size Base::GetSize() {
			if(autosize==AutosizeModes::None)
				return WidgetBase::GetSize();
			else
				return WidgetBase::GetSize();
		}

		void Base::containerenabledchanged(bool state) {
			//!TODO
		}

		void Base::focus_anim_finished() {
			if(focus.to==Blueprint::FT_None && focus_anim_loop)
				focus_anim.ResetProgress();

			if(focus.to==Blueprint::FT_None)
				return;

			focus_anim.Pause();
			focus.from=focus.to;
			focus.to=Blueprint::FT_None;

			if(next_focus!=Blueprint::FT_None) {
				Blueprint::FocusType v=next_focus;
				next_focus=Blueprint::FT_None;

				setfocus(v);
			}

			Draw();
		}

		void Base::state_anim_finished() {
			if(state.to==0 && state_anim_loop)
				state_anim.ResetProgress();

			if(state.to==0 && next_state==0)
				return;

			state_anim.Pause();

			if(state.to!=0) {
				state.from=state.to;
				state.to=0;
			}

			if(next_state!=0) {
				int v=next_style;

				next_state=0;
				setstate(v);
			}

			Draw();
		}

		void Base::style_anim_finished() {
			if(style.to==Blueprint::YT_None && style_anim_loop)
				style_anim.ResetProgress();

			if(style.to==Blueprint::YT_None && next_style==Blueprint::YT_None)
				return;

			style_anim.Pause();

			if(style.to!=Blueprint::YT_None) {
				style.from=style.to;
				style.to=Blueprint::YT_None;
			}

			if(next_style!=Blueprint::YT_None) {
				Blueprint::StyleType v=next_style, t=Blueprint::YT_None;

				if(style.from==Blueprint::Down && next_style==Blueprint::Normal) {
					triggerwait();

					return;
				}

				if(style.from!=Blueprint::Normal) {
					if(next_style==Blueprint::Disabled) {
						v=Blueprint::Normal;
						t=next_style;
					}
				}
				else if(style.from==Blueprint::Disabled) {
					if(next_style!=Blueprint::Normal) {
						v=Blueprint::Normal;
						t=next_style;
					}
				}

				next_style=t;
				setstyle(v);
			}

			Draw();
		}

		bool Base::detach(ContainerBase *container) {
			innerlayer.parent=NULL;
			return WidgetBase::detach(container);
		}

		void Base::located(ContainerBase* container, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order) {
			WidgetBase::located(container, w, Order);
			if(BaseLayer)
				BaseLayer->Add(innerlayer, 0);
		}

	}
}}
