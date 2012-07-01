#include "BitmapFont.h"
#include "File.h"
#include "../Engine/Graphics.h"
#include <algorithm>
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
		int chmap[256];
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

		RGBint cc=target->GetCurrentColor();

		unsigned int i;
		if(Shadow.Type==ShadowParams::Flat) {
			int x=X;
			target->SetCurrentColor(Shadow.Color);
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

		target->SetCurrentColor(color);
		for(i=0;i<text.length();i++) {
			Image *img=Characters[(unsigned char)text[i]];
			if(img) {
				img->Draw(target,X,Y);
				X+=img->GetWidth() + Seperator;
			}
		}

		target->SetCurrentColor(cc);
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


		if(Shadow.Type==ShadowParams::Flat)
			Print(target,x+Shadow.Offset.x,y+Shadow.Offset.y,w,text,Shadow.Color,align);
		else if(Shadow.Type==ShadowParams::DropShadow) {
			BitmapFont &shadow=Blur(Shadow.Blur);
			int sizediff=(shadow.TextWidth(" ")-TextWidth(" "))/2;
			shadow.Print(target, x+Shadow.Offset.x-sizediff, y+Shadow.Offset.y-sizediff, w, text, Shadow.Color, align);
		}

		RGBint cc=target->GetCurrentColor();
		target->SetCurrentColor(color);

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

		target->SetCurrentColor(cc);
	}
	void BitmapFont::Print(graphics::ColorizableImageTarget2D *target, int x, int y, int w, const std::string &text, graphics::RGBint color, EPrintData *Data, int DataLen, TextAlignment::Type Align, ShadowParams Shadow) {

		RGBint cc=target->GetCurrentColor();
		target->SetCurrentColor(color);
		
		int sizediff;
		BitmapFont *shadow;

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
									target->SetCurrentColor(Shadow.Color);
									img->Draw(target,l+Shadow.Offset.x,y+Shadow.Offset.y);
								}
								else if(Shadow.Type==ShadowParams::DropShadow) {
									target->SetCurrentColor(Shadow.Color);
									shadow->Characters[(unsigned char)' ']->Draw(target, l+Shadow.Offset.x-sizediff, y+Shadow.Offset.y-sizediff);
								}

								target->SetCurrentColor(color);
								img->Draw(target,l,y);
								dist+=img->GetWidth()+Seperator;
							}
						}
					}
					else if(text[j]!='\r') {
						Image *img=Characters[(unsigned char)text[j]];
						if(img) {
							if(Shadow.Type==ShadowParams::Flat) {
								target->SetCurrentColor(Shadow.Color);
								img->Draw(target,l+Shadow.Offset.x,y+Shadow.Offset.y);
							}
							else if(Shadow.Type==ShadowParams::DropShadow) {
								target->SetCurrentColor(Shadow.Color);
								shadow->Characters[(unsigned char)text[j]]->Draw(target, l+Shadow.Offset.x-sizediff, y+Shadow.Offset.y-sizediff);
							}

							target->SetCurrentColor(color);
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

		target->SetCurrentColor(cc);

	}
	void BitmapFont::Print_Test(int x, int y, int w, const std::string &text, EPrintData *Data, int DataLen, TextAlignment::Type Align) {
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
		Base::Prepare(main, file);
		this->file=&file;

		if(!noshadows) {
			Blur(1.f);
			Blur(1.6f);
		}
	}

	BitmapFont::~BitmapFont() {
		for(auto it=Shadows.begin();it!=Shadows.end();++it) {
			if(it->first!=0) {
				delete it->second;
				it->second=NULL;
			}
		}
	}

} }
