#include "TextboxElement.h"
#include "Textbox.h"
#include "Line.h"
#include "WidgetRegistry.h"

using namespace gge::resource;

namespace gge { namespace widgets {
	TextboxElement::TextboxElement(void) {
		Visual=NULL;
		Parent=NULL;
		SelectionRect=NULL;
		SelectionOffset=Bounds(0,0,0,0);
	}

	void TextboxElement::Prepare(gge::GGEMain &main, gge::resource::File &file) {
		ResourceBase::Prepare(main);
		Font	=dynamic_cast<BitmapFontResource*>(file->FindObject(font_guid));
		Sound	=dynamic_cast<SoundResource*>(file->FindObject(sound_guid));

		if(Subitems[0]->getGID()==GID_LINE) {
			Visual=new Line((LineResource*)Subitems[0]);
		}
		if(Subitems[0]->getGID()==GID_ANIMATION) {
			Visual=new ImageAnimation((AnimationResource*)Subitems[0]);
		}
		if(Subitems.getCount()>1) {
			if(Subitems[1]->getGID()==GID_LINE) {
				SelectionRect=new Line((LineResource*)Subitems[1]);
			}
			if(Subitems[1]->getGID()==GID_RECT) {
				SelectionRect=new ResizableRect((RectangleResource*)Subitems[1]);
			}
		}
	}

	TextboxElement &TextboxElement::ReadyAnimation(bool Backwards) {
		Visual->Reset(Backwards);

		return *this;
	}

	TextboxElement &TextboxElement::Reverse() {
		Visual->Reverse();

		return *this;
	}

	TextboxElement &TextboxElement::Draw(WidgetLayer &layer,graphics::Colorizable2DLayer &textlayer,string Caption) {
		int x=Offset.x;
		int y=Offset.y;
		
		int cx=Visual->Width(layer.W);
		int cy=Visual->Width(layer.H);
	
		if(!SelectionRect)
			SelectionRect=&WR.createSelectionFrame();

		textlayer.Clear();
		textlayer.X=x+TextMargins.Left;
		textlayer.Y=y+TextMargins.Top;
		textlayer.W=cx-(TextMargins.Right+TextMargins.Left);
		textlayer.H=cy-(TextMargins.Bottom+TextMargins.Top);

		TextAlignment halign;
		if(TextAlign & ALIGN_LEFT)
			halign=TEXTALIGN_LEFT;
		if(TextAlign & ALIGN_RIGHT)
			halign=TEXTALIGN_RIGHT;
		if(TextAlign & ALIGN_CENTER)
			halign=TEXTALIGN_CENTER;

		int fh=0;
		if(Font)
			fh=Font->FontHeight();
		int ty;
		int cury;
		if(TextAlign & ALIGN_TOP) {
			ty=y;
			cury=y;
		}
		if(TextAlign & ALIGN_BOTTOM) {
			ty=(cy-(TextMargins.Top+TextMargins.Bottom))-fh;
			cury=(cy-(TextMargins.Top+TextMargins.Bottom))-Parent->Caret->Height();
		}
		if(TextAlign & ALIGN_MIDDLE) {
			ty=((cy-(TextMargins.Top+TextMargins.Bottom))-fh)/2;
			cury=((cy-(TextMargins.Top+TextMargins.Bottom))-Parent->Caret->Height())/2;
		}

		int eprintcount=4;
		if(Parent->isFocussed() || Parent->AlwaysShowSelection)
			eprintcount=10;
		EPrintData eprint[10];

		if(Font) {
			int w=cx-(TextMargins.Right+TextMargins.Left);
			int cw=Parent->Caret->Width();


			eprint[0].Type=EMT_Wrap;
			eprint[0].In.value=0;
			
			eprint[1].Type=EMT_PositionDetect;
			eprint[1].CharPosition=0;
			
			eprint[2].Type=EMT_Spacing;
			eprint[2].In.position.x=cw;
			eprint[2].In.position.y=0;
			eprint[2].CharPosition=Parent->caretposition;
			
			eprint[3].Type=EMT_PositionDetect;
			eprint[3].CharPosition=Caption.length();
			
			eprint[4].Type=EMT_Color;
			eprint[4].CharPosition=Parent->selectstart;
			eprint[4].In.color=SelectionColor;
			
			eprint[5].Type=EMT_Color;
			eprint[5].CharPosition=Parent->selectend;
			eprint[5].In.color=ForeColor;
			
			eprint[6].Type=EMT_PositionDetect;
			eprint[6].CharPosition=Parent->selectstart;
			
			eprint[7].Type=EMT_PositionDetect;
			eprint[7].CharPosition=Parent->selectend;
			
			eprint[8].Type=EMT_ShadowColor;
			eprint[8].CharPosition=Parent->selectstart;
			eprint[8].In.color=SelectionShadow;

			eprint[9].Type=EMT_ShadowColor;
			eprint[9].CharPosition=Parent->selectend;
			eprint[9].In.color=ShadowColor;



			//if not active we will not show caret
			if(!Parent->isFocussed())
				eprint[2].Type=EMT_PositionDetect;

			//first test to determine whether scrolling is required
			Font->Print_Test(
				0,ty,w,
				Caption,eprint,eprintcount,halign
			);


			//scroll is needed
			if(eprint[3].Out.position.x>w || eprint[1].Out.position.x<0) {
				//test to obtain left aligned positions
				Font->Print_Test(
					0,ty,w,
					Caption,eprint,4,TEXTALIGN_LEFT
				);

		
				//if needed change scroll amount
				if((eprint[2].Out.position.x-1)+15>w+Parent->scroll) {
					Parent->scroll=((eprint[2].Out.position.x-1)+cw)-w+15;
					if(Parent->scroll+w>eprint[3].Out.position.x)
						Parent->scroll=eprint[3].Out.position.x-w;
				} else if((eprint[2].Out.position.x-1)-15<Parent->scroll) {
					Parent->scroll=(eprint[2].Out.position.x-1)-15;
					if(Parent->scroll<0) Parent->scroll=0;
				}

				Font->Print_Test(
					-Parent->scroll,ty,w,
					Caption,
					eprint,eprintcount,TEXTALIGN_LEFT
				);


				if(Parent->isEnabled() && Parent->selectstart!=Parent->selectend && Font && SelectionRect && (Parent->isFocussed() || Parent->AlwaysShowSelection))
					SelectionRect->DrawResized(textlayer, 
						(eprint[6].Out.position.x-Parent->scroll)-SelectionOffset.Left,
						textlayer.Y-SelectionOffset.Top,
						(eprint[7].Out.position.x-eprint[6].Out.position.x)+SelectionOffset.Left+SelectionOffset.Right,
						Font->FontHeight()+SelectionOffset.Top+SelectionOffset.Bottom,
						ALIGN_MIDDLE_CENTER);

				//print left aligned text
				Font->Print(
					textlayer,
					-Parent->scroll,ty,w,
					Caption,
					ForeColor,eprint,eprintcount,TEXTALIGN_LEFT,
					ShadowParams(ShadowParams::Flat,ShadowColor,ShadowOffset.x,ShadowOffset.y)
				);
				calign=TEXTALIGN_LEFT;
			} else { //no scrolling, print normal text
				if(Parent->isEnabled() && Parent->selectstart!=Parent->selectend && Font && SelectionRect && (Parent->isFocussed() || Parent->AlwaysShowSelection))
					SelectionRect->DrawResized(textlayer, 
						(eprint[6].Out.position.x)-SelectionOffset.Left,
						textlayer.Y-SelectionOffset.Top,
						(eprint[7].Out.position.x-eprint[6].Out.position.x)+SelectionOffset.Left+SelectionOffset.Right,
						Font->FontHeight()+SelectionOffset.Top+SelectionOffset.Bottom,
						ALIGN_MIDDLE_CENTER);

				Font->Print(
					textlayer,
					0,ty,w,
					Caption,
					ForeColor,eprint,eprintcount,halign,
					ShadowParams(ShadowParams::Flat,ShadowColor,ShadowOffset.x,ShadowOffset.y)
					
				);
				Parent->scroll=0;
				calign=halign;
			}

			//if focussed, draw caret
			if(Parent->isEnabled() && Parent->isFocussed())
				if(Caption!="")
					Parent->Caret->Draw(textlayer,(eprint[2].Out.position.x)-Parent->scroll,(int)cury+(eprint[2].Out.position.y));
				else
					Parent->Caret->Draw(textlayer,(eprint[2].Out.position.x-Parent->Caret->Width()/2)-Parent->scroll,(int)cury+(eprint[2].Out.position.y));

			//determine starting position of text
			lx=Parent->scroll;
		}

		Visual->DrawResized(layer,0,0,cx,cy,ALIGN_MIDDLE_CENTER);

		return *this;
	}

	int TextboxElement::detectChar(int x) {
		//x-=lx;
		EPrintData eprint[2];
		eprint[0].Type=EMT_Wrap;
		eprint[0].In.value=0;
		eprint[1].Type=EMT_CharacterDetect;
		eprint[1].In.position.x=x;

		Font->Print_Test(
			lx,0,Parent->width-(TextMargins.Right+TextMargins.Left),
			Parent->text,eprint,2,calign
		);

		return eprint[1].Out.value;
	}
} }
