#include "ButtonElement.h"

using namespace gge::resource;

namespace gge { namespace widgets {
	ButtonElement::ButtonElement(void) : font_guid(nullptr), sound_guid(nullptr) {
		Visual=NULL;
		TextAlign=ALIGN_MIDDLE_CENTER;
		IconAlign=ALIGN_MIDDLE_CENTER;
		TextMargins=Bounds(0,0,0,0);
		IconMargins=Bounds(0,0,0,0);
	}

	void ButtonElement::Prepare(gge::GGEMain &main, gge::resource::File &file) {
		ResourceBase::Prepare(main);
		if(Subitems.getFirstItem()->getGID()==GID_ANIMATION)
			Visual=new ImageAnimation((AnimationResource*)Subitems.getFirstItem());
		if(Subitems.getFirstItem()->getGID()==GID_RECT)
			Visual=new ResizableRect((RectangleResource*)Subitems.getFirstItem());
		if(Subitems.getFirstItem()->getGID()==GID_LINE)
			Visual=new Line((LineResource*)Subitems.getFirstItem());

		Font	=dynamic_cast<BitmapFontResource*>(file->FindObject(font_guid));
		Sound	=dynamic_cast<SoundResource*>(file->FindObject(sound_guid));
	}

	ButtonElement &ButtonElement::ReadyAnimation(bool Backwards) {
		Visual->Reset(Backwards);

		return *this;
	}

	ButtonElement &ButtonElement::Reverse() {
		Visual->Reverse();

		return *this;
	}

	ButtonElement &ButtonElement::Activate() {
		Visual->Play();

		return *this;
	}

	ButtonElement &ButtonElement::Deactivate() {
		Visual->Pause();

		return *this;
	}


	ButtonElement &ButtonElement::Draw(WidgetLayer &Target,graphics::Colorizable2DLayer &TextTarget,graphics::Colorizable2DLayer &IconTarget,string Caption,graphics::Buffered2DGraphic *Icon) {
		int X=Offset.x;
		int Y=Offset.y;
		int Width=Visual->Width(Target.W);
		int Height=Visual->Height(Target.H);

		TextTarget.Clear();
		Target.Clear();
		IconTarget.Clear();
		TextTarget.X=X+TextMargins.Left;
		TextTarget.Y=Y+TextMargins.Top;
		TextTarget.W=Width-(TextMargins.Right+TextMargins.Left);
		TextTarget.H=Height-(TextMargins.Bottom+TextMargins.Top);

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
		int tx,ty;
		if(TextAlign & ALIGN_TOP)
			ty=0;
		if(TextAlign & ALIGN_BOTTOM)
			ty=(Height-(TextMargins.Top+TextMargins.Bottom))-fh;
		if(TextAlign & ALIGN_MIDDLE)
			ty=((Height-(TextMargins.Top+TextMargins.Bottom))-fh)/2;

		if(Font)
			Font->Print(TextTarget,0,ty,Width-(TextMargins.Right+TextMargins.Left),Caption,ForeColor,halign, ShadowParams(ShadowParams::Flat, ShadowColor, ShadowOffset.x, ShadowOffset.y));

		if(Icon) {
			if(IconAlign & ALIGN_LEFT)
				tx=X+IconMargins.Left;
			if(IconAlign & ALIGN_RIGHT)
				tx=X+(Width-(IconMargins.Left+IconMargins.Right))-Icon->Texture.W+IconMargins.Left;
			if(IconAlign & ALIGN_CENTER)
				tx=X+((Width-(IconMargins.Left+IconMargins.Right))-Icon->Texture.W)/2+IconMargins.Left;

			if(IconAlign & ALIGN_TOP)
				ty=Y+IconMargins.Top;
			if(IconAlign & ALIGN_BOTTOM)
				ty=Y+(Height-(IconMargins.Top+IconMargins.Bottom))-Icon->Texture.H+IconMargins.Top;
			if(IconAlign & ALIGN_MIDDLE)
				ty=Y+((Height-(IconMargins.Top+IconMargins.Bottom))-Icon->Texture.H)/2+IconMargins.Top;

			Icon->Draw(IconTarget,tx,ty);
		}

		Visual->DrawResized(Target,X,Y,Width,Height,TextAlign);


		return *this;
	}
} }
