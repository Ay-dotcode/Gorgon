#include "CheckboxElement.h"
#include "Line.h"
#include "Rectangle.h"
#include "CheckboxBase.h"

using namespace gge::resource;

namespace gge { namespace widgets {
	CheckboxElement::CheckboxElement(void) : symbol(NULL), border(NULL), bordertemplate(NULL), Lines(1), base(NULL) {
	}

	void CheckboxElement::Prepare(gge::GGEMain &main, gge::resource::File &file) {
		ResourceBase::Prepare(main);
		Font	=(gge::Font)temp_font;
		Sound	=dynamic_cast<SoundResource*>(file->FindObject(sound_guid));
		symbol  =new ImageAnimation( dynamic_cast<AnimationResource*>(Subitems[0]) );

		if(Subitems.getCount()>1)
			bordertemplate=Subitems[1];

		if(bordertemplate) {
			if(bordertemplate->getGID()==GID_ANIMATION)
				border=new ImageAnimation(dynamic_cast<AnimationResource*>(bordertemplate));
			if(bordertemplate->getGID()==GID_RECT)
				border=new ResizableRect(dynamic_cast<RectangleResource*>(bordertemplate));
			if(bordertemplate->getGID()==GID_LINE)
				border=new Line(dynamic_cast<LineResource*>(bordertemplate));
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

	CheckboxElement &CheckboxElement::Draw(WidgetLayer &layer,graphics::Colorizable2DLayer &textlayer,string &caption) {
		SymbolIconOrderConstants symboliconorder=SymbolIconOrder;
		Margins cm; //current margin
		
		/*if(border) {
			cm += BorderMargin;

			if(AutoBorderWidth) {
				if(bordertemplate) {
					if(bordertemplate->getGID()==GID_RECT)
						BorderWidth=((RectangleResource*)bordertemplate)->getBorderWidth();
					else
						BorderWidth=Margins(0);
				} else
					BorderWidth=Margins(0);
			}

			textlayer.Clear();
			layer.Clear();

			border->DrawResized(
				layer, 
				cm.Left, cm.Top, 
				layer.W-cm.TotalX(), layer.H-cm.TotalY(), 
				ALIGN_MIDDLE_CENTER
			);

			cm += BorderWidth;
		}*/

		cm += ContentMargin;

		/*if( Lines==3 && (base->icon==NULL || symbol==NULL || symboliconorder==NoIcon || symboliconorder==NoSymbol || symboliconorder==NoSymbolNoIcon) )
			Lines=2;

		if( Lines==2  && base->icon==NULL && (symboliconorder==NoSymbol || symboliconorder==NoSymbolNoIcon) )
			Lines=1;

		if( Lines==2  && symbol==NULL && (symboliconorder==NoIcon || symboliconorder==NoSymbolNoIcon) )
			Lines=1;

		if(symboliconorder==NoSymbolNoIcon)
			Lines=1;
		
		//All have separate lines
		if(Lines==3) {
			if(symboliconorder == SymbolFirst) {
				symbol->DrawResized(
					layer, 
					cm.Left + SymbolMargin.Left, 
					cm.Top  + SymbolMargin.Top,
					layer.W - ( cm.TotalX() + SymbolMargin.TotalX() ),
					symbol->Height(),
					SymbolAlign
				);

				cm.AddToTop(symbol->Height(), SymbolMargin);

				base->icon->DrawResized(
					layer, 
					cm.Left + IconMargin.Left, 
					cm.Top  + IconMargin.Top,
					layer.W - ( cm.TotalX() + IconMargin.TotalX() ),
					base->icon->Height(),
					IconAlign
				);

				cm.AddToTop(base->icon->Height(), IconMargin);
			} else {
				base->icon->DrawResized(
					layer, 
					cm.Left + IconMargin.Left, 
					cm.Top  + IconMargin.Top,
					layer.W - ( cm.TotalX() + IconMargin.TotalX() ),
					base->icon->Height(),
					IconAlign
				);

				cm.AddToTop(base->icon->Height(), IconMargin);

				symbol->DrawResized(
					layer, 
					cm.Left + SymbolMargin.Left, 
					cm.Top  + SymbolMargin.Top,
					layer.W - ( cm.TotalX() + SymbolMargin.TotalX() ),
					symbol->Height(),
					SymbolAlign
				);

				cm.AddToTop(symbol->Height(), SymbolMargin);
			}
		} 


		//Symbol and/or Icon is in first line, text on the bottom
		if(Lines==2) {
			int lineheight=0;

			if(symbol && symboliconorder != NoSymbol) {
				if(symbol->Height()>lineheight)
					lineheight=symbol->Height();
			}
			if(base->icon && symboliconorder != NoIcon) {
				if(base->icon->Height()>lineheight)
					lineheight=base->icon->Height();
			}

			if(symbol && base->icon) {
				if( IconAlign&ALIGN_MASK_HORIZONTAL == SymbolFirst&ALIGN_MASK_HORIZONTAL ) {
					Margins lm=cm;

					if(symboliconorder == SymbolFirst) {
						
					}

				}
			}



			if(symboliconorder == SymbolFirst) {
				symbol->DrawResized(
					layer, 
					cm.Left + SymbolMargin.Left, 
					cm.Top  + SymbolMargin.Top,
					layer.W - ( cm.TotalX() + SymbolMargin.TotalX() ),
					symbol->Height(),
					SymbolAlign
				);

				cm.AddToTop(symbol->Height(), SymbolMargin);

				base->icon->DrawResized(
					layer, 
					cm.Left + IconMargin.Left, 
					cm.Top  + IconMargin.Top,
					layer.W - ( cm.TotalX() + IconMargin.TotalX() ),
					base->icon->Height(),
					IconAlign
				);

				cm.AddToTop(base->icon->Height(), IconMargin);
			} else {
				base->icon->DrawResized(
					layer, 
					cm.Left + IconMargin.Left, 
					cm.Top  + IconMargin.Top,
					layer.W - ( cm.TotalX() + IconMargin.TotalX() ),
					base->icon->Height(),
					IconAlign
				);

				cm.AddToTop(base->icon->Height(), IconMargin);

				symbol->DrawResized(
					layer, 
					cm.Left + SymbolMargin.Left, 
					cm.Top  + SymbolMargin.Top,
					layer.W - ( cm.TotalX() + SymbolMargin.TotalX() ),
					symbol->Height(),
					SymbolAlign
				);

				cm.AddToTop(symbol->Height(), SymbolMargin);
			}
		}*/
		
		int x=cm.Left;
		int y=cm.Top;
		int cx=layer.W-cm.TotalX();
		int cy=layer.H-cm.TotalY();
		if( symbol  && !(SymbolIconOrder==CheckboxElement::NoSymbol || SymbolIconOrder==CheckboxElement::NoSymbolNoIcon) ) {
			symbol->DrawResized(layer,cm.Left+IconMargin.Left,cm.Top+IconMargin.Top,cx-IconMargin.TotalX(),cy-IconMargin.TotalY(),IconAlign);

			if( Lines==1 ) {
				if( IconAlign&ALIGN_LEFT ) {
					textlayer.X=x+IconMargin.Left+symbol->Width()+TextMargin.Left;
					textlayer.Y=y+TextMargin.Top;
					textlayer.W=cx-(TextMargin.TotalX()+IconMargin.Left+symbol->Width());
					textlayer.H=cy-TextMargin.TotalY();
				} else if(IconAlign&ALIGN_RIGHT) {
					textlayer.X=x+TextMargin.Left;
					textlayer.Y=y+TextMargin.Top;
					textlayer.W=cx-(TextMargin.TotalX()+IconMargin.Left+symbol->Width());
					textlayer.H=cy-TextMargin.TotalY();
				} else {
					textlayer.X=x+TextMargin.Left;
					textlayer.Y=y+TextMargin.Top;
					textlayer.W=cx-TextMargin.TotalX();
					textlayer.H=cy-TextMargin.TotalY();
				}
			} else if( Lines == 2 ) {
				if( IconAlign&ALIGN_TOP ) {
					textlayer.X=x+TextMargin.Left;
					textlayer.Y=y+IconMargin.Top+symbol->Height()+TextMargin.Top;
					textlayer.W=cx-TextMargin.TotalX();
					textlayer.H=cy-(TextMargin.TotalY()+IconMargin.Top+symbol->Height());
				} else if(IconAlign&ALIGN_BOTTOM) {
					textlayer.X=x+TextMargin.Left;
					textlayer.Y=y+TextMargin.Top;
					textlayer.W=cx-TextMargin.TotalX();
					textlayer.H=cy-(TextMargin.TotalY()+IconMargin.Top+symbol->Height());
				} else {
					textlayer.X=x+TextMargin.Left;
					textlayer.Y=y+TextMargin.Top;
					textlayer.W=cx-TextMargin.TotalX();
					textlayer.H=cy-TextMargin.TotalY();
				}
			}
		} else {
			textlayer.X=x+TextMargin.Left;
			textlayer.Y=y+TextMargin.Top;
			textlayer.W=cx-TextMargin.TotalX();
			textlayer.H=cy-TextMargin.TotalY();
		}


		TextAlignment halign=TEXTALIGN_LEFT;
		if(TextAlign & ALIGN_LEFT)
			halign=TEXTALIGN_LEFT;
		if(TextAlign & ALIGN_RIGHT)
			halign=TEXTALIGN_RIGHT;
		if(TextAlign & ALIGN_CENTER)
			halign=TEXTALIGN_CENTER;

		int fh=Font.FontHeight();
		int ty;
		if(TextAlign & ALIGN_TOP)
			ty=0;
		if(TextAlign & ALIGN_BOTTOM)
			ty=textlayer.H-fh;
		if(TextAlign & ALIGN_MIDDLE)
			ty=(textlayer.H-fh)/2;

		textlayer.Clear();

		Font.Print(textlayer,0,ty,textlayer.W,caption,halign);

		return *this;
	}
} }
