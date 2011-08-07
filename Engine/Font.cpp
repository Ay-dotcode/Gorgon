#include "Font.h"
#include "../Resource/FontTheme.h"

using namespace gge::resource;
using namespace gge::graphics;

namespace gge {
	Font::Font(FontTheme &Theme, RGBint Color, FontStyle Style, ShadowParams Shadow) :
	  Theme(&Theme), Style(Style), Color(Color), Shadow(Shadow)
	{ }

	Font::Font(FontTheme *Theme, RGBint Color, FontStyle Style, ShadowParams Shadow) :
	  Theme(Theme), Style(Style), Color(Color), Shadow(Shadow)
	{ }

	Font::Font(const Font &f) :  
	  Theme(f.Theme), Style(f.Style), Color(f.Color), Shadow(f.Shadow)
	{ }

	Font::Font() :  
	  Theme(NULL), Style(Normal), Color(0x0), Shadow(ShadowParams())
	{ }

	Font &Font::operator =(const Font &f) {
		Theme=f.Theme;
		Style=f.Style;
		Color=f.Color;
		Shadow=f.Shadow;

		return *this;
	}
	Font &Font::operator =(const FontStyle f) {
		Style=f;

		return *this;
	}


	//Info functions
	int Font::FontHeight() {
		FontRenderer *r;
		if(r=getRenderer())
			return r->FontHeight();
		else
			return 0;
	}
	int Font::TextWidth(string Text) {
		FontRenderer *r;
		if(r=getRenderer())
			return r->TextWidth(Text);
		else
			return 0;
	}


	//Print Functions
	void Font::Print(ColorizableImageTarget2D *target, int X, int Y, string Text) {
		FontRenderer *r;
		if(r=getRenderer())
			r->Print(target, X, Y, Text, Color, Shadow);
	}
	void Font::Print(ColorizableImageTarget2D *target, int X, int Y, int W, string Text, TextAlignment::Type Align) {
		FontRenderer *r;
		if(r=getRenderer())
			r->Print(target, X, Y, W, Text, Color, Align, Shadow);
	}
	void Font::Print(ColorizableImageTarget2D *target, int X, int Y, int W, string Text, EPrintData *Data, int DataLen, TextAlignment::Type Align) {
		FontRenderer *r;
		if(r=getRenderer())
			r->Print(target, X, Y, W, Text, Color, Data, DataLen, Align, Shadow);
	}
	void Font::Print_Test(int X, int Y, int W, string Text, EPrintData *Data, int DataLen, TextAlignment::Type Align) {
		FontRenderer *r;
		if(r=getRenderer())
			r->Print_Test(X, Y, W, Text, Data, DataLen, Align);
	}

	FontRenderer *Font::getRenderer() { if(!Theme) return NULL; return Theme->DetermineRenderer(Style); }

	FontInitiator::operator Font() {
		return Font(dynamic_cast<FontTheme*>(file->FindObject(guid_theme)), Color, Style, Shadow);
	}


	FontInitiator Font::Load(File &File, std::istream &Data, int Size) {
		FontInitiator f;

		f.file=&File;

		f.guid_theme.Load(Data);
		f.Style=(Font::FontStyle)ReadFrom<int>(Data);
		ReadFrom(Data, f.Color);

		int gid, size;

		ReadFrom(Data, gid);
		ReadFrom(Data, size);

		f.Shadow=ShadowParams::Load(File,Data,size);

		return f;
	}

}