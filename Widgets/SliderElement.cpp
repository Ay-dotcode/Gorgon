#include "SliderElement.h"
#include "Slider.h"

namespace gorgonwidgets {
	SliderElement::SliderElement() {
		font_guid=NULL;
		sound_guid=NULL;

		Overlay=NULL;
		Rule=NULL;
		Symbol=NULL;
		Tick=NULL;
		
	}

	void SliderElement::setStyle(SliderStyles style) {
		this->style=style;

		switch(style) {
			case SS_Top:
			case SS_Bottom:
			case SS_Horizontal:
				dynamic_cast<Line*>(Rule)->isVertical=false;
				if(Overlay)
					dynamic_cast<Line*>(Overlay)->isVertical=false;
				break;
			case SS_Verticle:
			case SS_Left:
			case SS_Right:
				dynamic_cast<Line*>(Rule)->isVertical=true;
				if(Overlay)
					dynamic_cast<Line*>(Overlay)->isVertical=true;
				break;
		}
	}

	void SliderElement::Prepare(gge::GGEMain *main) {
		ResourceBase::Prepare(main);

		Font	=dynamic_cast<BitmapFontResource*>(file->FindObject(font_guid));
		Sound	=dynamic_cast<SoundResource*>(file->FindObject(sound_guid));

		int canim=0, cline=0;
		LinkedListIterator<ResourceBase> it=Subitems;
		ResourceBase *resource;
		while(resource=it) {
			if(resource->getGID()==GID_ANIMATION) {
				if(canim==0)
					Symbol=dynamic_cast<AnimationResource*>(resource)->getAnimation();
				else if(canim==1)
					Tick=dynamic_cast<AnimationResource*>(resource)->getAnimation();

				canim++;
			}
			if(resource->getGID()==GID_LINE) {
				if(cline==0)
					Rule=new Line(dynamic_cast<LineResource*>(resource));
				else if(cline==1) {
					Overlay=new Line(dynamic_cast<LineResource*>(resource));

					if(dynamic_cast<Line*>(Overlay)->Start==NULL) {
						delete Overlay;
						Overlay=NULL;
					}
				}

				cline++;
			}
		}
	}

	SliderElement &SliderElement::DrawRule(WidgetLayer &Target) {
			Rule->DrawResized(Target, 0, 0, Target.W, Target.H, ALIGN_MIDDLE_CENTER);

		return *this;
	}
	SliderElement &SliderElement::DrawOverlay(Basic2DLayer &Target) {
		int h,w;
		int y;

		if(!Overlay) return *this;
		Line*overlay=dynamic_cast<Line*>(Overlay);
		switch(style) {
			case SS_Right:
			case SS_Verticle:
			case SS_Left:
				h=Target.H;
				y=0;
				if(h<overlay->Start->Height() + overlay->End->Height()) {
					h=overlay->Start->Height() + overlay->End->Height();
					y=Target.H-h;
				}

				Overlay->DrawResized(Target, 0, y, Target.W, h, ALIGN_MIDDLE_CENTER);
				break;
			case SS_Bottom:
			case SS_Horizontal:
			case SS_Top:
				w=Target.W;
				if(w<overlay->Start->Width() + overlay->End->Width()) {
					w=overlay->Start->Width() + overlay->End->Width();
				}

				Overlay->DrawResized(Target, 0, 0, w, Target.H, ALIGN_MIDDLE_CENTER);
				break;
		}

		return *this;
	}
	gge::Rectangle SliderElement::RuleRectangle(WidgetLayer &Target, Point up, Point down, Point dist) {
		gge::Rectangle b;
		int h, w;
		h=Rule->Height(Target.H);
		w=Rule->Width(Target.W);
		switch(style) {
			case SS_Bottom:
				b.Left=down.x + dist.x;
				b.Width= w- (up.x + dist.x*2 + down.x);
				b.Top=RuleOffset;
				b.Height=h;
				break;
			case SS_Top:
				b.Top=Target.H-(RuleOffset+h);
				b.Height=h;
				b.Left=up.x + dist.x;
				b.Width=w - (up.x + dist.x*2 + down.x);
				break;
			case SS_Horizontal:
				b.Left=up.x + dist.x;
				b.Width=w - (up.x + dist.x*2 + down.x) ;
				b.Top=(Target.H-(h+RuleOffset))/2+RuleOffset;
				b.Height=h;
				break;
			case SS_Right:
				b.Top=up.y + dist.y;
				b.Height=h - (up.y + dist.y*2 + down.y) ;
				b.Left=RuleOffset;
				b.Width=w;
				break;
			case SS_Left:
				b.Top=up.y + dist.y;
				b.Height=h - (up.y + dist.y*2 + down.y) ;
				b.Left=Target.W-(RuleOffset+w);
				b.Width=w;
				break;
			case SS_Verticle:
				b.Top=up.y + dist.y;
				b.Height=h - (up.y + dist.y*2 + down.y) ;
				b.Left=(Target.W-(w+RuleOffset))/2+RuleOffset;
				b.Width=w;
				break;
		}

		return b;
	}
	SliderElement &SliderElement::DrawTickMarks(WidgetLayer &Target, float Distance) {
		float y,x;
		float w=Target.W;
		float h=Target.H;

		if(Distance==0)
			return *this;

		switch(style) {
			case SS_Right: 
			case SS_Left: 
			case SS_Verticle: 
			{
				for(y=0;y<h-1;y+=Distance)
					Tick->Draw(Target, 0, Round(y));
				
				y=h;
				Tick->Draw(Target, 0, y);
				

				break;
			}
			case SS_Top: 
			case SS_Bottom: 
			case SS_Horizontal: 
			{
				for(x=0;x<w-1;x+=Distance)
					Tick->Draw(Target, Round(x), 0);
				
				x=w;
				Tick->Draw(Target, x, 0);
				

				break;
			}
		}

		return *this;
	}
	SliderElement &SliderElement::DrawTickNumbers(Colorizable2DLayer &Target, float Distance, float Start, float Increment, float End, string Format) {
		float v, y, x;
		char temp[40];

		switch(style) {
			case SS_Right: 
			case SS_Left: 
			case SS_Verticle: 
			{
				y=Target.H-Font->FontHeight();
				for(v=Start;v<End;v+=Increment) {
					sprintf_s<40>(temp, Format.data(), v);

					Font->Print(Target, 0, Round(y), Target.W, temp, ForeColor, TextAlign, ShadowParams(ShadowParams::Flat,ShadowColor, ShadowOffset.x, ShadowOffset.y));
					y-=Distance;
				}
				sprintf_s<40>(temp, Format.data(), End);

				Font->Print(Target, 0, y, Target.W, temp, ForeColor, TextAlign, ShadowParams(ShadowParams::Flat,ShadowColor, ShadowOffset.x, ShadowOffset.y));
				y-=Distance;
				

				break;
			}
			case SS_Top: 
			case SS_Bottom: 
			case SS_Horizontal: 
			{
				x=0;
				for(v=Start;v<End;v+=Increment) {
					sprintf_s<40>(temp, Format.data(), v);

					/*if(v==Start && TextAlign==TEXTALIGN_CENTER)
						Font->Print(Target, x, 0, 0, temp, ForeColor, TEXTALIGN_LEFT, ShadowParams(ShadowTypes::Flat,ShadowColor, ShadowOffset.x, ShadowOffset.y));
					else*/
						Font->Print(Target, Round(x), 0, 0, temp, ForeColor, TextAlign, ShadowParams(ShadowParams::Flat,ShadowColor, ShadowOffset.x, ShadowOffset.y));
					x+=Distance;
				}
				sprintf_s<40>(temp, Format.data(), End);
				/*if(TextAlign==TEXTALIGN_CENTER)
					Font->Print(Target, Target->W, 0, 0, temp, ForeColor, TEXTALIGN_RIGHT, ShadowParams(ShadowTypes::Flat,ShadowColor, ShadowOffset.x, ShadowOffset.y));
				else*/
					Font->Print(Target, Target.W, 0, 0, temp, ForeColor, TextAlign, ShadowParams(ShadowParams::Flat,ShadowColor, ShadowOffset.x, ShadowOffset.y));
			}
		}

		return *this;
	}
	SliderElement &SliderElement::DrawTickNames(Colorizable2DLayer &Target, float Distance, Collection<SliderLocationName> &Texts, bool useLocations, float min, float max) {
		float x, y;

		if(useLocations) {
			switch(style) {
				case SS_Right: 
				case SS_Left: 
				case SS_Verticle: 
				{
					Texts.ResetIteration();
					SliderLocationName *t;
					while(t=Texts.next()) {
						y=Target.H-((t->value-min)/(max-min))*Target.H;
						Font->Print(Target, 0, y, Target.W, t->name, ForeColor, TextAlign, ShadowParams(ShadowParams::Flat,ShadowColor, ShadowOffset.x, ShadowOffset.y));
					}
					

					break;
				}
				case SS_Bottom: 
				case SS_Top: 
				case SS_Horizontal: 
				{
					x=0;
					Texts.ResetIteration();
					SliderLocationName *t;
					while(t=Texts.next()) {
						x=((t->value-min)/(max-min))*Target.W;
						/*if(x==0 && TextAlign==TEXTALIGN_CENTER)
							Font->Print(Target, x, 0, 0, *t, ForeColor, TEXTALIGN_LEFT, ShadowParams(ShadowTypes::Flat,ShadowColor, ShadowOffset.x, ShadowOffset.y));
						else if(Round(x)==Target->W && TextAlign==TEXTALIGN_CENTER)
							Font->Print(Target, x, 0, 0, *t, ForeColor, TEXTALIGN_RIGHT, ShadowParams(ShadowTypes::Flat,ShadowColor, ShadowOffset.x, ShadowOffset.y));
						else*/
							Font->Print(Target, x, 0, 0, t->name, ForeColor, TextAlign, ShadowParams(ShadowParams::Flat,ShadowColor, ShadowOffset.x, ShadowOffset.y));
					}
					

					break;
				}
			}
		} else {
			switch(style) {
				case SS_Right: 
				case SS_Left: 
				case SS_Verticle: 
				{
					y=Target.H;
					Texts.ResetIteration();
					SliderLocationName *t;
					while(t=Texts.next()) {

						Font->Print(Target, 0, Round(y), Target.W, t->name, ForeColor, TextAlign, ShadowParams(ShadowParams::Flat,ShadowColor, ShadowOffset.x, ShadowOffset.y));
						y-=Distance;
					}
					

					break;
				}
				case SS_Bottom: 
				case SS_Top: 
				case SS_Horizontal: 
				{
					x=0;
					Texts.ResetIteration();
					SliderLocationName *t;
					while(t=Texts.next()) {
						if(x==0 && TextAlign==TEXTALIGN_CENTER)
							Font->Print(Target, x, 0, 0, t->name, ForeColor, TEXTALIGN_LEFT, ShadowParams(ShadowParams::Flat,ShadowColor, ShadowOffset.x, ShadowOffset.y));
						else if(Round(x)==Target.W && TextAlign==TEXTALIGN_CENTER)
							Font->Print(Target, x, 0, 0, t->name, ForeColor, TEXTALIGN_RIGHT, ShadowParams(ShadowParams::Flat,ShadowColor, ShadowOffset.x, ShadowOffset.y));
						else
							Font->Print(Target, Round(x), 0, 0, t->name, ForeColor, TextAlign, ShadowParams(ShadowParams::Flat,ShadowColor, ShadowOffset.x, ShadowOffset.y));
						x+=Distance;
					}
					

					break;
				}
			}
		}

		return *this;
	}
	SliderElement &SliderElement::DrawLocatedTicks(WidgetLayer &Target, float Distance, Collection<SliderLocationName> &Texts, float min, float max) {
		float x, y;

		switch(style) {
			case SS_Right: 
			case SS_Left: 
			case SS_Verticle: 
			{
				Texts.ResetIteration();
				SliderLocationName *t;
				while(t=Texts.next()) {
					y=Target.H-((t->value-min)/(max-min))*Target.H;
					Tick->Draw(Target, 0, y);
				}
				

				break;
			}
			case SS_Bottom: 
			case SS_Top: 
			case SS_Horizontal: 
			{
				x=0;
				Texts.ResetIteration();
				SliderLocationName *t;
				while(t=Texts.next()) {
					x=((t->value-min)/(max-min))*Target.W;
					/*if(x==0 && TextAlign==TEXTALIGN_CENTER)
						Font->Print(Target, x, 0, 0, *t, ForeColor, TEXTALIGN_LEFT, ShadowParams(ShadowTypes::Flat,ShadowColor, ShadowOffset.x, ShadowOffset.y));
					else if(Round(x)==Target->W && TextAlign==TEXTALIGN_CENTER)
						Font->Print(Target, x, 0, 0, *t, ForeColor, TEXTALIGN_RIGHT, ShadowParams(ShadowTypes::Flat,ShadowColor, ShadowOffset.x, ShadowOffset.y));
					else*/
						Tick->Draw(Target, x, 0);
				}
				

				break;
			}
		}

		return *this;
	}
	SliderElement &SliderElement::DrawSymbol(WidgetLayer &Target) {
		Symbol->DrawResized(Target, 0,0,Target.W,Target.H,ALIGN_TOP_LEFT);

		return *this;
	}
	SliderElement &SliderElement::ReadyAnimation(bool Backwards) {
		Symbol->Reset();
		Rule->Reset();

		return *this;
	}
	SliderElement &SliderElement::Reverse() {
		Symbol->Reverse();
		Rule->Reverse();

		return *this;
	}
}