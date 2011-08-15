#include "CheckboxBase.h"
#include "..\..\Utils\Size2D.h"

using namespace gge::utils;
using namespace gge::resource;
using namespace gge::graphics;
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
				}

				h=max(nh,h);
			}

			if(lineborder) {
				h+=lineborder->BorderWidth.TotalY() + lineborder->Padding.TotalY();
				int margin=(lineborder->Margins.Top > prevymargin ? lineborder->Margins.TotalY() : lineborder->Margins.Bottom);
				h+=margin;
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


			//get id use line height and calculate new location
			if(Alignment::isMiddle(line->Align))
				y=y+(h-reqh)/2;
			else if(Alignment::isBottom(line->Align))
				y=y+(h-reqh);

			h=reqh;

			//determine content bounds
			Bounds exterior(0,y,BaseLayer->BoundingBox.Right,y+h);
			Bounds bounds  (0,y,innerlayer.BoundingBox.Right,y+h);

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

							totalmargin=(iconp->Margins.Left>pmargin ? iconp->Margins.TotalX()-pmargin : iconp->Margins.Right);
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

							totalmargin=(textp->Margins.Left>pmargin ? textp->Margins.TotalX()-pmargin : textp->Margins.Right);
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

							align=iconp->Align;
							x+=iconp->Margins.Right;
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

						if(textwrap)
							font->Print(innerlayer, Point(itembounds.Left,targetlocation.y),itembounds.Width(),text,TextAlignment::GetHorizontal(align));
						else
							font->Print(innerlayer, targetlocation,text);
					}

					break;

				case Blueprint::Symbol:
					//!TODO

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
			int y=inner.Top;
			for(int i=1;i<=3;i++) {
				if(lines[i]) {
					drawline(i, linetransitions[i], y, lineheights_org[i], lineheights[i], prevymargin);
					y+=lineheights[i];
				}
			}

			if(overlay) {
				overlay->DrawIn(BaseLayer, BaseLayer->BoundingBox);
			}



			delete[] groups;
		}

		void Base::setfocusstate(Blueprint::FocusType type) {
			if(!bp) {
				focus.from=type;
				return;
			}

			if(focus.from!=type || focus.to!=Blueprint::FT_None) {
				Blueprint::AnimationInfo info=bp->HasFocusAnimation(Blueprint::FocusMode(focus.from, type));

				if(info) {
					if(focus.from==type) {
						if(info.direction==focus_anim.GetSpeed()) {
							focus_queue.push(type);
							return;
						}
						else
							focus.from=focus.to;
					}
					else if(focus.to==type) {
						//should not be empty
						//if(!focus_queue.empty()) 
						focus_queue.pop();

						return;
					}
					
					if(info.duration==-2) {
						info.duration=-1;
						focus_anim_loop=true;
					}
					else
						focus_anim_loop=false;

					if(info.direction==Blueprint::Forward)
						focus_anim.SetPauseAt(info.duration);
					else
						focus_anim.SetPauseAt(0);

					focus_anim.SetSpeed((float)info.direction);

					focus.to=type;
				}
				else
					focus.from=type;
			}

			Draw();
		}

		bool Base::Focus() {
			if(!cangetfocus)
				return false;

			if(IsFocussed())
				return true;

			setfocusstate(Blueprint::Focused);

			return true;
		}
		bool Base::loosefocus(bool force) {
			if(!IsFocussed())
				return true;

			setfocusstate(Blueprint::NotFocused);

			return true;
		}

		void Base::Enable() {
			throw std::exception("The method or operation is not implemented.");
		}

		void Base::Disable() {
			throw std::exception("The method or operation is not implemented.");
		}

		utils::Size Base::GetSize() {
			if(autosize==AutosizeModes::None)
				return WidgetBase::GetSize();
			else
				return WidgetBase::GetSize();
		}

		void Base::focus_anim_finished() {
			if(focus.to==Blueprint::FT_None && focus_anim_loop)
				focus_anim.ResetProgress();

			focus.from=focus.to;
			focus.to=Blueprint::FT_None;

			if(!focus_queue.empty()) {
				Blueprint::FocusType v=focus_queue.front();
				focus_queue.pop();

				setfocusstate(v);
			}

			Draw();
		}

		void Base::containerenabledchanged(bool state) {

		}

		void Base::state_anim_finished() {

		}

		void Base::style_anim_finished() {

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
