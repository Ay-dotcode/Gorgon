#include "TextboxBase.h"
#include "..\..\Utils\Size2D.h"
#include "..\..\Resource\BitmapFontResource.h"

using namespace gge::utils;
using namespace gge::resource;
using namespace gge::graphics;
using namespace gge::resource;
using namespace std;


namespace gge { namespace widgets {
	namespace textbox {




		void Base::draw() {
			if(!BaseLayer)
				return;


			BaseLayer->Clear();
			innerlayer.Clear();
			overlayer.Clear();

			if(!bp)
				return;


			prepare();




			Bounds outer=BaseLayer->BoundingBox;
			Bounds inner=Bounds(0,0,outer.Width(), outer.Height());

			if(outerborder)
				inner=inner-(outerborder->BorderWidth+outerborder->Padding);

			innerlayer.BoundingBox=inner;
			overlayer.BoundingBox=Bounds(0,0,outer.Width(), outer.Height());

			//inner border


			if(outerborder) {
				outerborder->DrawIn(BaseLayer, 
					outerborder->Margins.Left, outerborder->Margins.Top,
					outer.Width()-outerborder->Margins.TotalX(),
					outer.Height()-outerborder->Margins.TotalY()
				);
			}


			//Draw text
		//TODO Wrap
			if(font) {
				if(text!="") {
					EPrintData eprint[3]={};

					int tw=font->TextWidth(prefix+text+suffix);
					int th=font->FontHeight();

					Point location;
					Point caretposition(0,0);
					Size caretsize;
					int caretimagew=0;
					if(caret) {
						caretimagew=caret->GetWidth();
						caretsize=caret->GetSize();
					}

					caretsize=bp->CaretPlace.GetSize(caretsize,caretsize)+bp->CaretPlace.Margins;

					//initial location
					if(tw<inner.Width()) {
						location=Alignment::CalculateLocation(bp->Align, Bounds(0,0,inner.Width(), inner.Height()), Size(tw,th));
					}
					else {
						location=Alignment::CalculateLocation(Alignment::setLeft(bp->Align), Bounds(0,0,inner.Width(), inner.Height()), Size(tw,th));
						location.x+=scroll.x;
					}

					//check for caret position
					eprint[0].Type=EPrintData::PositionDetect;
					eprint[0].CharPosition=caretlocation+(int)prefix.length();

					font->Print_Test(location, 0, prefix+text+suffix, eprint, 1);
					caretposition=Alignment::CalculateLocation(bp->CaretPlace.Align, utils::Rectangle(caretposition, caretsize.Width, font->FontBaseline()), caret->GetSize(), bp->CaretPlace.Margins);

					if(caretlocation==text.length() && tw+caretimagew>inner.Width()) {
						scroll.x=inner.Width()-(tw+caretimagew);
					}
					else if(caretlocation==0 && tw+caretimagew>inner.Width()) {
						scroll.x=0;
					}
					else {
						if(eprint[0].Out.position.x<-caretposition.x)
							scroll.x=-eprint[0].Out.position.x+location.x-caretposition.x;
						if(eprint[0].Out.position.x>-caretimagew+inner.Width()-caretposition.x)
							scroll.x=inner.Width()+location.x-eprint[0].Out.position.x-caretimagew-caretposition.x;
					}


					if(tw+caretimagew<=inner.Width()) {
						location=Alignment::CalculateLocation(bp->Align, Bounds(0,0,inner.Width(), inner.Height()), Size(tw,th));
						if(scroll.x>0)
							location.x+=scroll.x;
					}
					else {
						location=Alignment::CalculateLocation(Alignment::setLeft(bp->Align), Bounds(0,0,inner.Width(), inner.Height()), Size(tw,th));
						location.x+=scroll.x;
					}

					//after possible scrolling
					font->Print_Test(location, 0, prefix+text+suffix, eprint, 1);
					caretposition=eprint[0].Out.position;
					caretposition=Alignment::CalculateLocation(bp->CaretPlace.Align, utils::Rectangle(caretposition, caretsize.Width, font->FontBaseline()), caret->GetSize(), bp->CaretPlace.Margins);


					if(!IsFocused() || !caret) {
						font->Print(innerlayer, location, 0, prefix+text+suffix);
					}
					else {
						eprint[0].Type=EPrintData::Spacing;
						eprint[0].CharPosition=caretlocation+prefix.length();
						eprint[0].In.position.x=caretsize.Width;
						eprint[0].In.position.y=0;

						int selstart=min(selectionstart, caretlocation), selend=max(selectionstart, caretlocation);

						if(selstart!=selend && selection) {
							eprint[1].Type=EPrintData::PositionDetect;
							eprint[1].CharPosition=selstart+prefix.length();

							eprint[2].Type=EPrintData::PositionDetect;
							eprint[2].CharPosition=selend+prefix.length();

							font->Print_Test(location, 0, prefix+text+suffix, eprint, 3);

							Bounds selb(eprint[1].Out.position, eprint[2].Out.position);
							selb.Bottom+=th;

							selb=selb + selection->Padding;

							selection->DrawIn(innerlayer, selb);
						}



						font->Print(innerlayer, location, 0, prefix+text+suffix, eprint, 1);
						textlocation=location;

						caret->Draw(innerlayer, caretposition);
					}
				}
				else if(IsFocused()) {
					Point caretposition(0,0);
					Point location;
					int th=font->FontHeight();
					Size caretsize(0,0);
					if(caret) {
						caretsize=caret->GetSize();
					}

					location=Alignment::CalculateLocation(bp->Align, Bounds(0,0,inner.Width(), inner.Height()), Size(0,th));
					caretposition=Alignment::CalculateLocation(bp->CaretPlace.Align, utils::Rectangle(location, caretsize.Width, font->FontBaseline()), caret->GetSize(), bp->CaretPlace.Margins);
					caret->Draw(innerlayer, caretposition);
					textlocation=location;
				}
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

		void Base::prepare() {
			if(!unprepared)
				return;


			outerborder=NULL;
			overlay=NULL;
			font=NULL;

			if(!bp)
				return;

			BorderDataResource		*bprovider;
			bool transition;

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



			font=bp->GetFont(style, transition);


			unprepared=false;
		}

		bool Base::MouseEvent(input::mouse::Event::Type event, utils::Point location, int amount) {
			using namespace input::mouse;

			if(!IsEnabled())
				return WidgetBase::MouseEvent(event, location, amount);

			static Point lastlocation(0,0);

			if(event==Event::Left_Down) {
				if(font) {
					EPrintData eprint;
					eprint.Type=EPrintData::CharacterDetect;
					eprint.In.position=location;

					font->Print_Test(textlocation, 0, prefix+text+suffix, &eprint, 1);

					if(input::keyboard::Modifier::Current==input::keyboard::Modifier::Shift)
						setselection(selectionstart, eprint.Out.value-prefix.length());
					else
						setcaretlocation(eprint.Out.value-prefix.length());

					mdown=true;
					lastlocation=location;
				}
			}

			if(event==Event::Move && mdown) {
				if(lastlocation!=location) {

					EPrintData eprint;
					eprint.Type=EPrintData::CharacterDetect;
					eprint.In.position=location;

					font->Print_Test(textlocation, 0, text, &eprint, 1);

					setselection(selectionstart, eprint.Out.value-prefix.length());

					lastlocation=location;
				}
			}

			if(event==Event::Left_Up) {
				mdown=false;
			}

			if(event==Event::Over) {
				if(!IsFocused()) {
					playsound(widgets::Blueprint::Normal, widgets::Blueprint::Hover);

					setstyle(widgets::Blueprint::Hover);
				}
				mhover=true;
			}
			if(event==Event::Out) {
				if(!IsFocused()) {
					playsound(widgets::Blueprint::Hover, widgets::Blueprint::Normal);

					setstyle(widgets::Blueprint::Normal);
				}
				mhover=true;
			}

			if(vscroll.allow && event==Event::VScroll) {
				vscrollby(-amount);
			}

			return WidgetBase::MouseEvent(event, location, amount);
		}

		void Base::adjustscrolls() {

			//int yscrollrange=scrollinglayer.BoundingBox.Height()-innerlayer.BoundingBox.Height();
			//if(yscrollrange<0)
			//	yscrollrange=0;

			//int pmax=vscroll.bar.Max;
			//vscroll.bar.Max=yscrollrange;


			//if(vscroll.show && (!vscroll.autohide || vscroll.bar.Max!=0)) {
			//	scrollmargins.Right=vscroll.bar.GetWidth()+padding.Right;
			//	//if(innerborder)
			//	vscroll.bar.SetHeight(innerlayer.GetHeight());
			//	vscroll.bar.Show();
			//}
			//else {
			//	scrollmargins=Margins(0);
			//	vscroll.bar.Hide();
			//}


			//adjustlayers();
			//Draw();

			//vscroll.bar.Move(innerlayer.BoundingBox.Right+padding.Right, innerlayer.BoundingBox.Top);
		}

		bool Base::KeyboardEvent(input::keyboard::Event::Type event, input::keyboard::Key Key) {
			using namespace gge::input::keyboard;
			
			if(event==Event::Down && Modifier::Current==Modifier::None) {
				if(Key==KeyCodes::Left) {
					setcaretlocation(caretlocation-1);

					if(bp) {
						if(bp->TypeSound)
							WidgetBase::playsound(bp->TypeSound);
					}
					
					return true;
				}
				if(Key==KeyCodes::Right) {
					setcaretlocation(caretlocation+1);

					if(bp) {
						if(bp->TypeSound)
							WidgetBase::playsound(bp->TypeSound);
					}

					return true;
				}
				if(Key==KeyCodes::Home) {
					setcaretlocation(0);

					if(bp) {
						if(bp->TypeSound)
							WidgetBase::playsound(bp->TypeSound);
					}

					return true;
				}
				if(Key==KeyCodes::End) {
					setcaretlocation(text.length());

					if(bp) {
						if(bp->TypeSound)
							WidgetBase::playsound(bp->TypeSound);
					}

					return true;
				}
				if(Key==KeyCodes::Delete) {

					if(bp) {
						if(bp->TypeSound)
							WidgetBase::playsound(bp->TypeSound);
					}

					if(caretlocation!=selectionstart) {
						if(selectionstart<caretlocation) {
							text.erase(selectionstart, caretlocation-selectionstart);
							caretlocation=selectionstart;

							Draw();
						}
						else {
							text.erase(caretlocation, selectionstart-caretlocation);
							selectionstart=caretlocation;

							Draw();
						}

						textchanged();
					}
					else if(caretlocation<(int)text.length()) {
						text.erase(caretlocation, 1);

						Draw();

						textchanged();
					}
				}
			}
			if(event==Event::Down && Modifier::Current==Modifier::Shift) {
				if(Key==KeyCodes::Left) {
					setselection(selectionstart, caretlocation-1);

					if(bp) {
						if(bp->TypeSound)
							WidgetBase::playsound(bp->TypeSound);
					}

					return true;
				}
				if(Key==KeyCodes::Right) {
					setselection(selectionstart, caretlocation+1);

					if(bp) {
						if(bp->TypeSound)
							WidgetBase::playsound(bp->TypeSound);
					}

					return true;
				}
				if(Key==KeyCodes::Home) {
					setselection(selectionstart, 0);

					if(bp) {
						if(bp->TypeSound)
							WidgetBase::playsound(bp->TypeSound);
					}

					return true;
				}
				if(Key==KeyCodes::End) {
					setselection(selectionstart, text.length());

					if(bp) {
						if(bp->TypeSound)
							WidgetBase::playsound(bp->TypeSound);
					}

					return true;
				}
			}

			if(event==Event::Char && !Modifier::Check()) {
				if(bp) {
					if(bp->TypeSound)
						WidgetBase::playsound(bp->TypeSound);
				}

				if(Key==KeyCodes::Backspace) {
					if(caretlocation!=selectionstart) {
						if(selectionstart<caretlocation) {
							text.erase(selectionstart, caretlocation-selectionstart);
							caretlocation=selectionstart;

							Draw();
						}
						else {
							text.erase(caretlocation, selectionstart-caretlocation);
							selectionstart=caretlocation;

							Draw();
						}

						textchanged();
					}
					else if(caretlocation) {
						setcaretlocation(caretlocation-1);
						text.erase(caretlocation, 1);

						textchanged();
					}

					return true;
				}
				else {

					if(Key >= 32) {
						if(caretlocation!=selectionstart) {
							if(selectionstart<caretlocation) {
								text.erase(selectionstart, caretlocation-selectionstart);
								caretlocation=selectionstart;
							}
							else {
								text.erase(caretlocation, selectionstart-caretlocation);
								selectionstart=caretlocation;
							}
						}

						string s(1, char(Key));

						validatetext(s);

						if(s!="") {
							text=text.insert(caretlocation, s);

							setcaretlocation(caretlocation+1);

							textchanged();
						}

						return true;
					}
				}
			}

			if(event==Event::Down && Modifier::Current==Modifier::Ctrl) {
				if(Key=='a' || Key=='A') {
					setselection(0,text.length());

					return true;
				}
				else if(Key=='v' || Key=='V') {
					string s=os::GetClipbardText();
					validatetext(s);

					if(s!="") {
						if(caretlocation!=selectionstart) {
							if(selectionstart<caretlocation) {
								text.erase(selectionstart, caretlocation-selectionstart);
								caretlocation=selectionstart;
							}
							else {
								text.erase(caretlocation, selectionstart-caretlocation);
								selectionstart=caretlocation;
							}
						}

						text=text.insert(caretlocation, s);
						setcaretlocation(caretlocation+s.length());

						textchanged();
					}

					return true;
				}
				else if(Key=='c' || Key=='C') {
					if(caretlocation!=selectionstart) {
						if(selectionstart<caretlocation) {
							os::SetClipboardText(text.substr(selectionstart, caretlocation-selectionstart));
						}
						else {
							os::SetClipboardText(text.substr(caretlocation, selectionstart-caretlocation));
						}
					}

					return true;
				}
				else if(Key=='x' || Key=='X') {
					if(caretlocation!=selectionstart) {
						if(selectionstart<caretlocation) {
							os::SetClipboardText(text.substr(selectionstart, caretlocation-selectionstart));
							text.erase(selectionstart, caretlocation-selectionstart);
							caretlocation=selectionstart;

							Draw();
						}
						else {
							os::SetClipboardText(text.substr(caretlocation, selectionstart-caretlocation));
							text.erase(caretlocation, selectionstart-caretlocation);
							selectionstart=caretlocation;

							Draw();
						}

						textchanged();
					}

					return true;
				}
			}

			return false;
		}

		void Base::SetBlueprint(const widgets::Blueprint &bp) {
			if(this->bp==&bp)
				return;

			utils::CheckAndDelete(caret);

			this->bp=static_cast<const Blueprint*>(&bp);
			if(WidgetBase::size.Width==0)
				SetWidth(this->bp->DefaultSize.Width);
			if(WidgetBase::size.Height==0)
				SetHeight(this->bp->DefaultSize.Height);

			for(auto i=BorderCache.begin();i!=BorderCache.end();++i)
				utils::CheckAndDelete(i->second);

			for(auto i=ImageCache.begin();i!=ImageCache.end();++i)
				utils::CheckAndDelete(i->second);

			if(this->bp) {
				this->pointer=bp.Pointer;

				if(this->bp->Scroller)
					vscroll.bar.SetBlueprint(*this->bp->Scroller);

				if(this->bp->Caret)
					caret=&this->bp->Caret->CreateAnimation(true);

				if(this->bp->Selection)
					selection=&this->bp->Selection->CreateAnimation(true);
			}

			Draw();
		}

		void Base::playsound(Blueprint::StyleType stylefrom, Blueprint::StyleType styleto) {
			if(bp) {
				if(bp->Mapping[stylefrom][styleto] && bp->Mapping[stylefrom][styleto]->Sound) {
					WidgetBase::playsound(bp->Mapping[stylefrom][styleto]->Sound);
				}
			}
		}

		bool Base::Focus() {
			if(!IsEnabled() || !IsVisible())
				return false;

			if(!IsFocused()) {
				playsound(widgets::Blueprint::Normal, Blueprint::Active);
			}

			setstyle(widgets::Blueprint::Focused_Style);

			return WidgetBase::Focus();
		}

		void Base::Enable() {
			if(!IsEnabled()) {
				playsound(widgets::Blueprint::Disabled, widgets::Blueprint::Normal);
				setstyle(widgets::Blueprint::Normal);
				WidgetBase::Enable();
			}
		}

		void Base::Disable() {
			if(IsEnabled()) {
				playsound(widgets::Blueprint::Normal, widgets::Blueprint::Disabled);
				setstyle(widgets::Blueprint::Disabled);

				WidgetBase::Disable();
			}
		}

		bool Base::loosefocus(bool force) {
			if(IsFocused()) {
				playsound(widgets::Blueprint::Active, widgets::Blueprint::Active);
			}

			if(mhover)
				setstyle(widgets::Blueprint::Hover);
			else
				setstyle(widgets::Blueprint::Normal);

			return true;
		}


	}
}}

