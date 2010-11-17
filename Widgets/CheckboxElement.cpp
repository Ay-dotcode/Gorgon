#include "CheckboxElement.h"
#include "Line.h"
#include "Rectangle.h"

namespace gorgonwidgets {
	CheckboxElement::CheckboxElement(void) : symbol(NULL), border(NULL), bordertemplate(NULL), lines(1) {
	}

	void CheckboxElement::Prepare(gge::GGEMain *main) {
		ResourceBase::Prepare(main);
		Font=(BitmapFontResource*)file->FindObject(font_guid);
		Sound=(SoundResource*)file->FindObject(sound_guid);
		symbol=new ImageAnimation((AnimationResource*)Subitems[0]);

		if(Subitems.getCount()>1)
			bordertemplate=Subitems[1];

		if(bordertemplate) {
			if(bordertemplate->getGID()==GID_ANIMATION)
				border=new ImageAnimation((AnimationResource*)bordertemplate);
			if(bordertemplate->getGID()==GID_RECT)
				border=new ResizableRect((RectangleResource*)bordertemplate);
			if(bordertemplate->getGID()==GID_LINE)
				border=new Line((LineResource*)bordertemplate);
		}
	}

	CheckboxElement &CheckboxElement::ReadyAnimation(bool Backwards) {
		symbol->Reset(Backwards);

		return *this;
	}

	CheckboxElement &CheckboxElement::Reverse() {
		symbol->Reverse();

		return *this;
	}

	CheckboxElement &CheckboxElement::Draw(WidgetLayer &layer,Colorizable2DLayer &textlayer,string &caption) {
		int x=Offset.x;
		int y=Offset.y;

		int cx=layer.W-x;
		int cy=layer.H-y;

		textlayer.Clear();
		layer.Clear();

		if(border)
			border->DrawResized(layer, x,y, cx,cy, ALIGN_MIDDLE_CENTER);
		
		if( symbol ) {
			symbol->DrawResized(layer,x+IconMargins.Left,y+IconMargins.Top,cx-IconMargins.TotalX(),cy-IconMargins.TotalY(),IconAlign);

			if( lines==1 ) {
				if( IconAlign&ALIGN_LEFT ) {
					textlayer.X=x+IconMargins.Left+symbol->Width()+TextMargins.Left;
					textlayer.Y=y+TextMargins.Top;
					textlayer.W=cx-(TextMargins.TotalX()+IconMargins.Left+symbol->Width());
					textlayer.H=cy-TextMargins.TotalY();
				} else if(IconAlign&ALIGN_RIGHT) {
					textlayer.X=x+TextMargins.Left;
					textlayer.Y=y+TextMargins.Top;
					textlayer.W=cx-(TextMargins.TotalX()+IconMargins.Left+symbol->Width());
					textlayer.H=cy-TextMargins.TotalY();
				} else {
					textlayer.X=x+TextMargins.Left;
					textlayer.Y=y+TextMargins.Top;
					textlayer.W=cx-TextMargins.TotalX();
					textlayer.H=cy-TextMargins.TotalY();
				}
			} else if( lines == 2 ) {
				if( IconAlign&ALIGN_TOP ) {
					textlayer.X=x+TextMargins.Left;
					textlayer.Y=y+IconMargins.Top+symbol->Height()+TextMargins.Top;
					textlayer.W=cx-TextMargins.TotalX();
					textlayer.H=cy-(TextMargins.TotalY()+IconMargins.Top+symbol->Height());
				} else if(IconAlign&ALIGN_BOTTOM) {
					textlayer.X=x+TextMargins.Left;
					textlayer.Y=y+TextMargins.Top;
					textlayer.W=cx-TextMargins.TotalX();
					textlayer.H=cy-(TextMargins.TotalY()+IconMargins.Top+symbol->Height());
				} else {
					textlayer.X=x+TextMargins.Left;
					textlayer.Y=y+TextMargins.Top;
					textlayer.W=cx-TextMargins.TotalX();
					textlayer.H=cy-TextMargins.TotalY();
				}
			}
		} else {
			textlayer.X=x+TextMargins.Left;
			textlayer.Y=y+TextMargins.Top;
			textlayer.W=cx-TextMargins.TotalX();
			textlayer.H=cy-TextMargins.TotalY();
		}


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
		if(TextAlign & ALIGN_TOP)
			ty=0;
		if(TextAlign & ALIGN_BOTTOM)
			ty=textlayer.H-fh;
		if(TextAlign & ALIGN_MIDDLE)
			ty=(textlayer.H-fh)/2;

		if(Font)
			Font->Print(textlayer,0,ty,textlayer.W,caption,ForeColor,halign,ShadowParams(ShadowTypes::Flat,ShadowColor,ShadowOffset.x,ShadowOffset.y));

		return *this;
	}
}