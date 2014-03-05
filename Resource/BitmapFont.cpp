#include "BitmapFont.h"
#include "File.h"
#include "../Engine/Graphics.h"
#include <algorithm>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#ifdef MSVC
#	include <ppl.h>
#endif

#ifndef MAX_CHAR_DETECTS
#define MAX_CHAR_DETECTS	10
#endif

#pragma GCC diagnostic ignored "-Wuninitialized"


using namespace gge::graphics;
using namespace gge::utils;
using namespace std;

namespace gge { namespace resource {
  
	BitmapFont *LoadBitmapFontResource(File &File, std::istream &Data, int Size) {
		BitmapFont *font=new BitmapFont;
		int chmap[256]={};
		int cpos=0,i;

		font->Seperator=1;

		int target=Data.tellg()+Size;
		while(Data.tellg()<target) {
			int gid,size;
			ReadFrom(Data, gid);
			ReadFrom(Data, size);

			if(gid==GID::Font_Charmap) {
				Data.read((char*)chmap, 4*256);
			}
			else if(gid==GID::Guid) {
				font->guid.LoadLong(Data);
			}
			else if(gid==GID::SGuid) {
				font->guid.Load(Data);
			}
			else if(gid==GID::Font_Image || gid==GID::Image) {
				Image *img=LoadImageResource(File,Data,size);

				for(i=0;i<256;i++)
					if(chmap[i]==cpos)
						font->Characters[i]=img;

				font->Subitems.Add(img);
				cpos++;
			}
			else if(gid==GID::Font_Props) {
				ReadFrom(Data,font->Seperator);
				ReadFrom(Data,font->VerticalSpacing);
				ReadFrom(Data,font->Baseline);
			}
			else {
				EatChunk(Data,size);
			}
		}

		return font;
	}
	void BitmapFont::Print(graphics::ColorizableImageTarget2D *target, int X, int Y, const std::string &text, graphics::RGBint color, ShadowParams Shadow) {
		if(text=="") return;
		if(!Characters[0]) return;

		RGBint cc=target->GetCurrentColor();

		unsigned int i;
		if(Shadow.Type==ShadowParams::Flat) {
			int x=X;
			target->SetCurrentColor(gge::graphics::RGBfloat(Shadow.Color));
			for(i=0;i<text.length();i++) {
				Image *img=Characters[(unsigned char)text[i]];
				img->Draw(target,x+Shadow.Offset.x,Y+Shadow.Offset.y);
				x+=img->GetWidth() + Seperator;
			}
		}
		else if(Shadow.Type==ShadowParams::DropShadow) {
			BitmapFont &shadow=Blur(Shadow.Blur);
			int sizediff=(shadow.TextWidth(" ")-TextWidth(" "))/2;
			shadow.Print(target, X+Shadow.Offset.x-sizediff, Y+Shadow.Offset.y-sizediff, text, Shadow.Color);
		}

		target->SetCurrentColor(gge::graphics::RGBfloat(color));
		for(i=0;i<text.length();i++) {
			Image *img=Characters[(unsigned char)text[i]];
			if(img) {
				img->Draw(target,X,Y);
				X+=img->GetWidth() + Seperator;
			}
		}

		target->SetCurrentColor(gge::graphics::RGBfloat(cc));
	}

	void BitmapFont::Print(graphics::ColorizableImageTarget2D *target, int x, int y, int w, const std::string &text, graphics::RGBint color, TextAlignment::Type align, ShadowParams Shadow) {
		unsigned i;
		int l=x;
		int lstart=0,lword=0;
		int llen=0;
		int lww=0;
		//int fh=FontHeight();
		int j;
		bool nextline=false;
		bool wrap=true;

		if(text=="") return;
		if(!Characters[0]) return;


		if(Shadow.Type==ShadowParams::Flat)
			Print(target,x+Shadow.Offset.x,y+Shadow.Offset.y,w,text,Shadow.Color,align);
		else if(Shadow.Type==ShadowParams::DropShadow) {
			BitmapFont &shadow=Blur(Shadow.Blur);
			int sizediff=(shadow.TextWidth(" ")-TextWidth(" "))/2;
			shadow.Print(target, x+Shadow.Offset.x-sizediff, y+Shadow.Offset.y-sizediff, w, text, Shadow.Color, align);
		}

		RGBint cc=target->GetCurrentColor();
		target->SetCurrentColor(gge::graphics::RGBfloat(color));

		if(w==0) {
			/*align=TextAlignment::Left;
			w=10000;*/
			wrap=false;
		}

		for(i=0;i<text.length();i++) {
			if(text[i]=='\n') {
				lword=i-1;
				lww=llen;
				nextline=true;
			}
			else {
				llen+=Characters[(unsigned char)text[i]]->GetWidth()+Seperator;

				if(text[i]==' ' || text[i]==',' || text[i]==')') {
					lword=i;
					lww=llen;
				}

				if(llen>w && wrap) {
					if(lword<=lstart) {
						lword=i;
						lww=llen;
					}

					nextline=true;
				}
				if(i==text.length()-1) {
					lword=i;
					lww=llen;

					nextline=true;
				}
			}

			if(nextline) {
				switch(align) {
				case TextAlignment::Left:
					l=x;
					break;
				case TextAlignment::Center:
					l=x+(w-lww)/2;
					break;
				case TextAlignment::Right:
					l=x+(w-lww);
					break;

				}

				for(j=lstart;j<lword+1;j++) {
					if(text[j]=='\t') {
						Image *img=Characters[(unsigned char)' '];
						int i;
						for(i=0;i<Tabsize;i++) {
							img->Draw(target, l, y);
							l+=img->GetWidth()+Seperator;
						}
					}
					if(text[j]!='\r') {
						Image *img=Characters[(unsigned char)text[j]];
						if(img) {
							img->Draw(target, l, y);
							l+=img->GetWidth()+Seperator;
						}
					}
				}

				if((int)text.length()-1==lword)
					;
				else if(text[lword+1]=='\n')
					lword++;

				lstart=lword+1;
				i=lword;
				llen=0;
				y+=VerticalSpacing;
				nextline=false;
			}
		}

		target->SetCurrentColor(gge::graphics::RGBfloat(cc));
	}
	void BitmapFont::Print(graphics::ColorizableImageTarget2D *target, int x, int y, int w, const std::string &text, graphics::RGBint color, EPrintData *Data, int DataLen, TextAlignment::Type Align, ShadowParams Shadow) {
		if(!Characters[0]) return;

		RGBint cc=target->GetCurrentColor();
		target->SetCurrentColor(graphics::RGBfloat(color));
		
		int sizediff=0;
		BitmapFont *shadow=nullptr;

		if(Shadow.Type==ShadowParams::DropShadow) {
			shadow=&Blur(Shadow.Blur);
			sizediff=(shadow->TextWidth(" ")-TextWidth(" "))/2;
		}

		if(text=="") {
			int d;
			int xpos=0;
			if(Align==TextAlignment::Center)
				xpos=w/2;
			else if(Align==TextAlignment::Right)
				xpos=w;

			for(d=0;d<DataLen;d++) {
				switch(Data[d].Type) {
				case EPrintData::PositionDetect:
					Data[d].Out.position.x=xpos;
					Data[d].Out.position.y=0;
					break;
				case EPrintData::Spacing:
					Data[d].Out.position.x=xpos;
					Data[d].Out.position.y=0;
					break;
				}
			}
			return;
		}

		int i,j,d;
		//int sx=x;
		//int sy=y;
		//int datc=0;
		int l=x;
		int lstart=0,lword=0;
		int llen=0;//line length
		int lww=0;
		bool nowrap=false;

		if(w<0) { w=-w; nowrap=true; }
		//bool done;
		//int fh=FontHeight();
		//int *data=(int*)Data;//!???
		bool nextline=false;
		struct {int x;EPrintData*data;} chardetectxs[MAX_CHAR_DETECTS];
		int cchardetectxs=0;

		for(d=0;d<DataLen;d++) {
			switch(Data[d].Type) {
			case EPrintData::Wrap:
				nowrap=!Data[d].In.value;
				break;
			case EPrintData::CharacterDetect:
				if(cchardetectxs<MAX_CHAR_DETECTS) {
					Data[d].Out.value=text.length();
					chardetectxs[cchardetectxs].x=Data[d].In.position.x;
					chardetectxs[cchardetectxs].data=Data+d;
					cchardetectxs++;
				}
				break;
			}
		}


		if(w==0)
			nowrap=true;

		switch(Align) {
		case TextAlignment::Left:
			l=x;
			break;
		case TextAlignment::Center:
			l=x+w/2;
			break;
		case TextAlignment::Right:
			l=x+w;
			break;
		}
		for(i=0;text[i];i++) {
			for(d=0;d<DataLen;d++) {
				if(Data[d].CharPosition==i) {
					switch(Data[d].Type) {
					case EPrintData::Spacing:
						llen+=Data[d].In.position.x-Seperator; 
						y+=Data[d].In.position.y; 
						break;
					}
				}
			}
			if(text[i]=='\n') {
				lword=i-1;
				lww=llen;
				nextline=true;
			}
			else {
				if(Characters[(unsigned char)text[i]]) {
					llen+=Characters[(unsigned char)text[i]]->GetWidth()+Seperator;
				}

				if(text[i]==' ' || text[i]==',' || text[i]==')') {
					lword=i;
					lww=llen;
				}

				if(llen>w && !nowrap) {
					if(lword<=lstart) {
						lword=i;
						lww=llen;
					}

					nextline=true;
				}
				if(!text[i+1]) {
					lword=i;
					lww=llen;

					nextline=true;
				}
			}

			if(nextline) {
				switch(Align) {
				case TextAlignment::Left:
					l=x;
					break;
				case TextAlignment::Center:
					l=x+(w-lww)/2;
					break;
				case TextAlignment::Right:
					l=x+(w-lww);
					break;
				}

				for(j=lstart;j<lword+1;j++) {

					for(d=0;d<DataLen;d++) {
						if(Data[d].CharPosition==j) {
							switch(Data[d].Type) {
							case EPrintData::Spacing:
								Data[d].Out.position.x=l;
								Data[d].Out.position.y=y;

								l+=Data[d].In.position.x-Seperator; 
								y+=Data[d].In.position.y; 
								break;
							case EPrintData::PositionDetect:
								Data[d].Out.position.x=l;
								Data[d].Out.position.y=y;
								break;
							case EPrintData::Color:
								color=Data[d].In.color;

								break;
							case EPrintData::ShadowColor:
								Shadow.Color=Data[d].In.color;

								break;
							}
						}
					}
					int dist=0;
					if(text[j]=='\t') {
						Image *img=Characters[(unsigned char)' '];
						if(img) {
							int i;
							for(i=0;i<Tabsize;i++) {
								if(Shadow.Type==ShadowParams::Flat) {
									target->SetCurrentColor(graphics::RGBfloat(Shadow.Color));
									img->Draw(target,l+Shadow.Offset.x,y+Shadow.Offset.y);
								}
								else if(Shadow.Type==ShadowParams::DropShadow) {
									target->SetCurrentColor(graphics::RGBfloat(Shadow.Color));
									shadow->Characters[(unsigned char)' ']->Draw(target, l+Shadow.Offset.x-sizediff, y+Shadow.Offset.y-sizediff);
								}

								target->SetCurrentColor(graphics::RGBfloat(color));
								img->Draw(target,l,y);
								dist+=img->GetWidth()+Seperator;
							}
						}
					}
					else if(text[j]!='\r') {
						Image *img=Characters[(unsigned char)text[j]];
						if(img) {
							if(Shadow.Type==ShadowParams::Flat) {
								target->SetCurrentColor(graphics::RGBfloat(Shadow.Color));
								img->Draw(target,l+Shadow.Offset.x,y+Shadow.Offset.y);
							}
							else if(Shadow.Type==ShadowParams::DropShadow) {
								target->SetCurrentColor(graphics::RGBfloat(Shadow.Color));
								shadow->Characters[(unsigned char)text[j]]->Draw(target, l+Shadow.Offset.x-sizediff, y+Shadow.Offset.y-sizediff);
							}

							target->SetCurrentColor(graphics::RGBfloat(color));
							img->Draw(target,l,y);
							dist=img->GetWidth()+Seperator;
						}
					}
					for(d=0;d<cchardetectxs;d++) {
						if(chardetectxs[d].x<l+dist/2) {//TODO y
							chardetectxs[d].data->Out.value=j;
						}
					}
					l+=dist;

				}

				if(text[lword+1]=='\n')
					lword++;

				lstart=lword+1;
				i=lword;
				llen=0;
				if(!nowrap)
					y+=VerticalSpacing;
				nextline=false;
			}
		}
		for(d=0;d<DataLen;d++) {
			if(Data[d].CharPosition==i) {
				switch(Data[d].Type) {
				case EPrintData::Spacing:
					Data[d].Out.position.x=l;
					Data[d].Out.position.y=y;

					l+=Data[d].In.position.x-Seperator; 
					y+=Data[d].In.position.y; 
					break;
				case EPrintData::PositionDetect:
					Data[d].Out.position.x=l;
					Data[d].Out.position.y=y;
					break;
				}
			}
		}	

		target->SetCurrentColor(graphics::RGBfloat(cc));

	}
	void BitmapFont::Print_Test(int x, int y, int w, const std::string &text, EPrintData *Data, int DataLen, TextAlignment::Type Align) {
		if(!Characters[0]) return;

		if(text=="") {
			int d;
			int xpos=0;
			if(Align==TextAlignment::Center)
				xpos=w/2;
			else if(Align==TextAlignment::Right)
				xpos=w;

			for(d=0;d<DataLen;d++) {
				switch(Data[d].Type) {
				case EPrintData::PositionDetect:
					Data[d].Out.position.x=xpos;
					Data[d].Out.position.y=0;
					break;
				case EPrintData::Spacing:
					Data[d].Out.position.x=xpos;
					Data[d].Out.position.y=0;
					break;
				}
			}
			return;
		}

		int i,j,d;
		int l=x;
		int lstart=0,lword=0;
		int llen=0;//line length
		int lww=0;
		bool nowrap=false;

		if(w<0) { w=-w; nowrap=true; }
		//bool done;
		//int fh=FontHeight();
		//int *data=(int*)Data; //!???
		bool nextline=false;
		struct {int x;EPrintData*data;} chardetectxs[MAX_CHAR_DETECTS];
		int cchardetectxs=0;

		for(d=0;d<DataLen;d++) {
			switch(Data[d].Type) {
			case EPrintData::Wrap:
				nowrap=!Data[d].In.value;
				break;
			case EPrintData::CharacterDetect:
				if(cchardetectxs<MAX_CHAR_DETECTS) {
					Data[d].Out.value=text.length();
					chardetectxs[cchardetectxs].x=Data[d].In.position.x;
					chardetectxs[cchardetectxs].data=Data+d;
					cchardetectxs++;
				}
				break;
			}
		}

		if(w==0)
			nowrap=true;

		switch(Align) {
		case TextAlignment::Left:
			l=x;
			break;
		case TextAlignment::Center:
			l=x+w/2;
			break;
		case TextAlignment::Right:
			l=x+w;
			break;
		}
		for(i=0;text[i];i++) {
			for(d=0;d<DataLen;d++) {
				if(Data[d].CharPosition==i) {
					switch(Data[d].Type) {
					case EPrintData::Spacing:
						llen+=Data[d].In.position.x-Seperator; 
						y+=Data[d].In.position.y; 
						break;
					}
				}
			}
			if(text[i]=='\n') {
				lword=i-1;
				lww=llen;
				nextline=true;
			}
			else {
				if(Characters[(unsigned char)text[i]]) {
					llen+=Characters[(unsigned char)text[i]]->GetWidth()+Seperator;
				}

				if(text[i]==' ' || text[i]==',' || text[i]==')') {
					lword=i;
					lww=llen;
				}

				if(llen>w && !nowrap) {
					if(lword<=lstart) {
						lword=i;
						lww=llen;
					}

					nextline=true;
				}
				if(!text[i+1]) {
					lword=i;
					lww=llen;

					nextline=true;
				}
			}

			if(nextline) {
				switch(Align) {
				case TextAlignment::Left:
					l=x;
					break;
				case TextAlignment::Center:
					l=x+(w-lww)/2;
					break;
				case TextAlignment::Right:
					l=x+(w-lww);
					break;
				}

				for(j=lstart;j<lword+1;j++) {
					for(d=0;d<DataLen;d++) {
						if(Data[d].CharPosition==j) {
							switch(Data[d].Type) {
							case EPrintData::Spacing:
								Data[d].Out.position.x=l/*-sx*/;
								Data[d].Out.position.y=y/*-sy*/;

								l+=Data[d].In.position.x-Seperator; 
								y+=Data[d].In.position.y; 
								break;
							case EPrintData::PositionDetect:
								Data[d].Out.position.x=l/*-sx*/;
								Data[d].Out.position.y=y/*-sy*/;
								break;
							case EPrintData::Color:

								break;
							case EPrintData::ShadowColor:

								break;
							}
						}
					}
					int dist=0;
					if(text[j]=='\t') {
						Image *img=Characters[(unsigned char)' '];
						if(img) {
							int i;
							for(i=0;i<Tabsize;i++) {
								dist+=img->GetWidth()+Seperator;
							}
						}
					}
					else if(text[j]!='\r') {
						Image *img=Characters[(unsigned char)text[j]];
						if(img) {
							dist=img->GetWidth()+Seperator;
						}
					}
					for(d=0;d<cchardetectxs;d++) {
						if(chardetectxs[d].x<l+dist/2 && j<chardetectxs[d].data->Out.value) {//!y??
							chardetectxs[d].data->Out.value=j;
						}
					}
					l+=dist;

				}

				if(text[lword+1]=='\n')
					lword++;

				lstart=lword+1;
				i=lword;
				llen=0;
				if(!nowrap)
					y+=VerticalSpacing;
				nextline=false;
			}
		}
		for(d=0;d<DataLen;d++) {
			if(Data[d].CharPosition==i) {
				switch(Data[d].Type) {
				case EPrintData::Spacing:
					Data[d].Out.position.x=l;
					Data[d].Out.position.y=y;

					l+=Data[d].In.position.x-Seperator; 
					y+=Data[d].In.position.y; 
					break;
				case EPrintData::PositionDetect:
					Data[d].Out.position.x=l;
					Data[d].Out.position.y=y;
					break;
				}
			}
		}	
	}

	int BitmapFont::TextHeight(const std::string &text, int w) {
		if(!Characters[0]) return 0;

		unsigned i;
		int l=0;
		int lstart=0,lword=0;
		int llen=0;
		//int lww=0;
		//int fh=FontHeight();
		int j;
		int y=0;
		bool nextline=false;
		bool wrap=true;

		if(text=="") return 0;


		if(w==0) {
			/*align=TextAlignment::Left;
			w=10000;*/
			wrap=false;
		}

		for(i=0;i<text.length();i++) {
			if(text[i]=='\n') {
				lword=i-1;
				nextline=true;
			}
			else {
				if(Characters[(unsigned char)text[i]]) {
					llen+=Characters[(unsigned char)text[i]]->GetWidth()+Seperator;
				}

				if(text[i]==' ' || text[i]==',' || text[i]==')') {
					lword=i;
				}

				if(llen>w && wrap) {
					if(lword<=lstart) {
						lword=i;
					}

					nextline=true;
				}
				if(i==text.length()-1) {
					lword=i;

					nextline=true;
				}
			}

			if(nextline) {
				l=0;

				for(j=lstart;j<lword+1;j++) {
					if(text[j]=='\t') {
						Image *img=Characters[(unsigned char)' '];
						if(img) {
							int i;
							for(i=0;i<Tabsize;i++) {
								l+=img->GetWidth()+Seperator;
							}
						}
					}
					if(text[j]!='\r') {
						Image *img=Characters[(unsigned char)text[j]];
						if(img) {
							l+=img->GetWidth()+Seperator;
						}
					}
				}

				if((int)text.length()-1==lword)
					;
				else if(text[lword+1]=='\n')
					lword++;

				lstart=lword+1;
				i=lword;
				llen=0;
				y+=VerticalSpacing;
				nextline=false;
			}
		}

		return y;
	}

  bool BitmapFont::Import(std::string fontname,  int size, char start, char end) {
    return import(fontname, size, start, end, [] () -> Image* { return new Image; });
  }

	bool BitmapFont::import(std::string fontname,  int size, char start, char end, std::function<Image*()> allocator) {
		if(start > end) {
			return false;
		}

		FT_Library instance;
		FT_Face face;
		int error;

		error = FT_Init_FreeType(&instance);
		if(error) {
			return false;
		}

		FT_New_Face(instance, fontname.c_str(), 0, &face);
		if (error) {
			return false;
		}


		error = FT_Set_Pixel_Sizes(face, 0, size);
		if(error) {
			return false;
		}


		std::vector<FT_Glyph> all(end-start+1);
		std::vector<utils::Size> sizes(end-start+1);
		std::vector<utils::Point> starts(end-start+1);

		Baseline=0;
		int vsize=0;
		int xdist=0;
		VerticalSpacing=size*face->height/face->units_per_EM;
		for(int ch = start; ch <= end; ch++) {
			unsigned int index = FT_Get_Char_Index(face, ch);

			error = FT_Load_Glyph(face, index, FT_LOAD_RENDER);
			if(error) {
				return false;
			}

			error = FT_Get_Glyph(face->glyph, &all[ch-start]);
			if(error) {
				return false;
			}

			if(Baseline<face->glyph->bitmap_top)
				Baseline = face->glyph->bitmap_top;

			sizes[ch-start].Width=std::max<int>(face->glyph->metrics.horiAdvance/64,face->glyph->bitmap.width);
			sizes[ch-start].Height=face->glyph->bitmap.rows;
			starts[ch-start]=utils::Point(face->glyph->bitmap_left, face->glyph->bitmap_top);
		}

		for(int i=0;i<end-start+1;i++) {
			int totalsize=(Baseline-starts[i].y)+sizes[i].Height;
			if(totalsize>vsize)
				vsize=totalsize;
		}

		for(int ch = start; ch <= end; ch++) {
			int size=sizes[ch-start].Width;
			if(size==0)
				size=5;

			auto img = allocator();
			this->Characters[ch] = img;
			Subitems.Add(img);
			img->Resize(size, vsize, gge::graphics::ColorMode::ARGB);
			memset(img->getdata().GetBuffer(), 0, img->getdata().GetSize());

			if(sizes[ch-start].Width==0 || all[ch-start]->format!=FT_GLYPH_FORMAT_BITMAP || !((FT_BitmapGlyph)all[ch-start])->bitmap.width || !((FT_BitmapGlyph)all[ch-start])->bitmap.rows) {
				continue;
			}
			
			auto &bitmap = ((FT_BitmapGlyph)all[ch-start])->bitmap;

			int ystart=Baseline-starts[ch-start].y;
			int xstart=starts[ch-start].x;
			for(int y = 0; y < bitmap.rows; y++) {
				for(int x = 0; x < bitmap.width; x++) {
          (*img)(x, y+ystart, 0) = 255;
          (*img)(x, y+ystart, 1) = 255;
          (*img)(x, y+ystart, 2) = 255;
          (*img)(x, y+ystart, 3) = bitmap.buffer[y * bitmap.pitch + x];
				}
			}
			FT_Done_Glyph(all[ch-start]);
		}
		for(int i=0;i<255;i++) {
			if(i<start || i>end)
				Characters[i]=Characters[start];
		}

		return true;
	}

	BitmapFont & BitmapFont::Blur(float amount, int windowsize/*=-1*/) {
		if(Shadows[amount])
			return *Shadows[amount];

		BitmapFont *font=new BitmapFont;

		if(windowsize==-1)
			windowsize=max(1,int(amount*1.5));

		font->Seperator=Seperator-windowsize*2;
		font->VerticalSpacing=VerticalSpacing;
		font->Baseline=Baseline+windowsize;

		vector<Base*> newitems;
		newitems.resize(Subitems.GetCount());

#ifdef MSVC
		Concurrency::parallel_for(0,Subitems.GetCount(), [&](int ind) {

			Image *img=dynamic_cast<Image*>(&Subitems[ind]);

			if(img) {
				newitems[ind]=&img->Blur(amount, windowsize);
			}
			else {
				newitems[ind]=&Subitems[ind];
			}
		});
#else
		for(int ind=0;ind<Subitems.GetCount();ind++) {

			Image *img=dynamic_cast<Image*>(&Subitems[ind]);

			if(img) {
				newitems[ind]=&img->Blur(amount, windowsize);
			}
			else {
				newitems[ind]=&Subitems[ind];
			}
		}
#endif

		for(int i=0;i<Subitems.GetCount();i++) {
			font->Subitems.Add(newitems[i], font->Subitems.HighestOrder()+1);
		}

		for(int i=0;i<256;i++) {
			int loc=Subitems.FindLocation(Characters[i]);
			if(loc>=0)
				font->Characters[i]=dynamic_cast<Image*>(font->Subitems(loc));
		}

		Shadows[amount]=font;
		font->noshadows=true;
		font->Prepare(Main, *file);
		return *font;
	}

	void BitmapFont::Prepare(GGEMain &main, File &file) {
		if(!noshadows) {
			Blur(1.f);
			Blur(1.6f);
		}

		if(noatlas) {
			Base::Prepare(main, file);
		}
		else {
			Image *baseatlas=new Image;
			destructionlist.Add(baseatlas);
			utils::OrderedCollection<Image> imagelist;

			for(auto &item : Subitems) {
				if(item.GetGID()==GID::Image) {
					imagelist.Add(dynamic_cast<Image&>(item));
				}
			}

			auto ret=baseatlas->CreateLinearAtlas(imagelist);
			baseatlas->Prepare(main, file);
			utils::OrderedCollection<Image> images;
			images=baseatlas->CreateAtlasImages(ret);

			int ind=0;
			for(auto &im : imagelist) {
				for(int i=0;i<256;i++) {
					if(Characters[i]==&im) {
						Characters[i]=&images[ind];
					}
				}
				destructionlist.Add(images[ind]);
				ind++;
			}
		}
	}

	BitmapFont::~BitmapFont() {
		for(auto it=Shadows.begin();it!=Shadows.end();++it) {
			if(it->first!=0) {
				delete it->second;
				it->second=NULL;
			}
		}

		destructionlist.Destroy();
	}

} }
