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

	Font &Font::operator =(Font &f) {
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
	void Font::Print(I2DColorizableGraphicsTarget *target, int X, int Y, string Text) {
		FontRenderer *r;
		if(r=getRenderer())
			r->Print(target, X, Y, Text, Color, Shadow);
	}
	void Font::Print(I2DColorizableGraphicsTarget *target, int X, int Y, int W, string Text, TextAlignment Align) {
		FontRenderer *r;
		if(r=getRenderer())
			r->Print(target, X, Y, W, Text, Color, Align, Shadow);
	}
	void Font::Print(I2DColorizableGraphicsTarget *target, int X, int Y, int W, string Text, EPrintData *Data, int DataLen, TextAlignment Align) {
		FontRenderer *r;
		if(r=getRenderer())
			r->Print(target, X, Y, W, Text, Color, Data, DataLen, Align, Shadow);
	}
	void Font::Print_Test(int X, int Y, int W, string Text, EPrintData *Data, int DataLen, TextAlignment Align) {
		FontRenderer *r;
		if(r=getRenderer())
			r->Print_Test(X, Y, W, Text, Data, DataLen, Align);
	}

	FontRenderer *Font::getRenderer() { if(!Theme) return NULL; return Theme->DetermineRenderer(Style); }

	FontInitiator::operator Font() {
		return Font(dynamic_cast<FontTheme*>(file->FindObject(guid_theme)), Color, Style, Shadow);
	}


	FontInitiator Font::Load(ResourceFile* file,FILE* gfile,int sz) {
		FontInitiator f;

		f.file=file;

		f.guid_theme=new Guid(gfile);
		fread(&f.Style,4,1,gfile);
		fread(&f.Color,4,1,gfile);

		int gid, size;

		fread(&gid, 4,1, gfile);
		fread(&size,4,1, gfile);

		f.Shadow=ShadowParams::Load(file,gfile,size);

		return f;
	}

}