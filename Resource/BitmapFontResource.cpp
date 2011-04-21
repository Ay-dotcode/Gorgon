#include "BitmapFontResource.h"
#include "ResourceFile.h"

#ifndef MAX_CHAR_DETECTS
#define MAX_CHAR_DETECTS	10
#endif

namespace gge { namespace resource {
	ResourceBase *LoadBitmapFontResource(resource::ResourceFile* File, FILE* Data, int Size) {
		BitmapFontResource *font=new BitmapFontResource;
		int chmap[256];
		int cpos=0,i;
		font->Seperator=1;

		int tpos=ftell(Data)+Size;
		while(ftell(Data)<tpos) {
			int gid,size;
			fread(&gid,1,4,Data);
			fread(&size,1,4,Data);

			if(gid==GID_FONT_CHARMAP) {
				fread(chmap,256,4,Data);
			}
			else if(gid==GID_GUID) {
				font->guid=new Guid(Data);
			}
			else if(gid==GID_FONT_IMAGE) {
				ImageResource *img=(ImageResource*)LoadImageResource(File,Data,size);

				for(i=0;i<256;i++)
					if(chmap[i]==cpos)
						font->Characters[i]=img;

				font->Subitems.Add(img);
				cpos++;
			}
			else if(gid==GID_FONT_SEPERATOR) {
				fread(&font->Seperator,1,2,Data);
			}
			else {
				EatChunk(Data,size);
			}
		}

		return font;
	}
	void BitmapFontResource::Print(graphics::I2DColorizableGraphicsTarget *target, int X, int Y, string text, graphics::RGBint color, ShadowParams Shadow) {
		if(text=="") return;

		unsigned int i;
		if(Shadow.Type==ShadowParams::Flat) {
			int x=X;
			for(i=0;i<text.length();i++) {
				ImageResource *img=Characters[text[i]];
				img->DrawColored(target,x+Shadow.Offset.x,Y+Shadow.Offset.y,Shadow.Color);
				x+=img->getWidth() + Seperator;
			}
		}
		for(i=0;i<text.length();i++) {
			ImageResource *img=Characters[text[i]];
			img->DrawColored(target,X,Y,color);
			X+=img->getWidth() + Seperator;
		}
	}

	void BitmapFontResource::Print(graphics::I2DColorizableGraphicsTarget *target, int x, int y, int w, string text, graphics::RGBint color, TextAlignment align, ShadowParams Shadow) {
		int i;
		int l=x;
		int lstart=0,lword=0;
		int llen=0;
		int lww=0;
		int fh=FontHeight();
		int j;
		bool nextline=false;
		bool wrap=true;

		if(text=="") return;

		if(Shadow.Type==ShadowParams::Flat)
			Print(target,x+Shadow.Offset.x,y+Shadow.Offset.y,w,text,Shadow.Color,align);

		if(w==0) {
			/*align=TEXTALIGN_LEFT;
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
				llen+=Characters[(unsigned char)text[i]]->getWidth()+Seperator;

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
				case TEXTALIGN_LEFT:
					l=x;
					break;
				case TEXTALIGN_CENTER:
					l=x+(w-lww)/2;
					break;
				case TEXTALIGN_RIGHT:
					l=x+(w-lww);
					break;

				}

				for(j=lstart;j<lword+1;j++) {
					if(text[j]=='\t') {
						ImageResource *img=Characters[(unsigned char)' '];
						int i;
						for(i=0;i<Tabsize;i++) {
							img->DrawColored(target, l, y, color);
							l+=img->getWidth()+Seperator;
						}
					}
					if(text[j]!='\r') {
						ImageResource *img=Characters[(unsigned char)text[j]];
						img->DrawColored(target, l, y, color);
						l+=img->getWidth()+Seperator;
					}
				}

				if(text[lword+1]=='\n')
					lword++;

				lstart=lword+1;
				i=lword;
				llen=0;
				y+=fh*VerticalSpacing;
				nextline=false;
			}
		}
	}
	void BitmapFontResource::Print(graphics::I2DColorizableGraphicsTarget *target, int x, int y, int w, string text, graphics::RGBint color, EPrintData *Data, int DataLen, TextAlignment Align, ShadowParams Shadow) {
		if(text=="") {
			int d;
			int xpos=0;
			if(Align==TEXTALIGN_CENTER)
				xpos=w/2;
			else if(Align==TEXTALIGN_RIGHT)
				xpos=w;

			for(d=0;d<DataLen;d++) {
				switch(Data[d].Type) {
				case EMT_PositionDetect:
					Data[d].Out.position.x=xpos;
					Data[d].Out.position.y=0;
					break;
				case EMT_Spacing:
					Data[d].Out.position.x=xpos;
					Data[d].Out.position.y=0;
					break;
				}
			}
			return;
		}

		int i,j,d;
		int sx=x;
		int sy=y;
		int datc=0;
		int l=x;
		int lstart=0,lword=0;
		int llen=0;//line length
		int lww=0;
		bool nowrap=false;

		if(w<0) { w=-w; nowrap=true; }
		bool done;
		int fh=FontHeight();
		int *data=(int*)Data;
		bool nextline=false;
		struct {int x;EPrintData*data;} chardetectxs[MAX_CHAR_DETECTS];
		int cchardetectxs=0;

		for(d=0;d<DataLen;d++) {
			switch(Data[d].Type) {
			case EMT_Wrap:
				nowrap=!Data[d].In.value;
				break;
			case EMT_CharacterDetect:
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
		case TEXTALIGN_LEFT:
			l=x;
			break;
		case TEXTALIGN_CENTER:
			l=x+w/2;
			break;
		case TEXTALIGN_RIGHT:
			l=x+w;
			break;
		}
		for(i=0;text[i];i++) {
			for(d=0;d<DataLen;d++) {
				if(Data[d].CharPosition==i) {
					switch(Data[d].Type) {
					case EMT_Spacing:
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
				llen+=Characters[(unsigned char)text[i]]->getWidth()+Seperator;

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
				case TEXTALIGN_LEFT:
					l=x;
					break;
				case TEXTALIGN_CENTER:
					l=x+(w-lww)/2;
					break;
				case TEXTALIGN_RIGHT:
					l=x+(w-lww);
					break;
				}

				for(j=lstart;j<lword+1;j++) {
					done=0;

					for(d=0;d<DataLen;d++) {
						if(Data[d].CharPosition==j) {
							switch(Data[d].Type) {
							case EMT_Spacing:
								Data[d].Out.position.x=l-sx;
								Data[d].Out.position.y=y-sy;

								l+=Data[d].In.position.x-Seperator; 
								y+=Data[d].In.position.y; 
								break;
							case EMT_PositionDetect:
								Data[d].Out.position.x=l-sx;
								Data[d].Out.position.y=y-sy;
								break;
							case EMT_Color:
								color=Data[d].In.color;

								break;
							case EMT_ShadowColor:
								Shadow.Color=Data[d].In.color;

								break;
							}
						}
					}
					int dist=0;
					if(text[j]=='\t') {
						ImageResource *img=Characters[(unsigned char)' '];
						int i;
						for(i=0;i<Tabsize;i++) {
							img->DrawColored(target,l,y,color);
							dist=img->getWidth()+Seperator;
						}
					}
					if(text[j]!='\r') {
						ImageResource *img=Characters[(unsigned char)text[j]];
						if(Shadow.Type==ShadowParams::Flat)
							img->DrawColored(target,l+Shadow.Offset.x,y+Shadow.Offset.y,Shadow.Color);

						img->DrawColored(target,l,y,color);
						dist=img->getWidth()+Seperator;
					}
					for(d=0;d<cchardetectxs;d++) {
						if(chardetectxs[d].x<(l-sx)+dist/2) {
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
					y+=fh*VerticalSpacing;
				nextline=false;
			}
		}
		for(d=0;d<DataLen;d++) {
			if(Data[d].CharPosition==i) {
				switch(Data[d].Type) {
				case EMT_Spacing:
					Data[d].Out.position.x=l-sx;
					Data[d].Out.position.y=y-sy;

					l+=Data[d].In.position.x-Seperator; 
					y+=Data[d].In.position.y; 
					break;
				case EMT_PositionDetect:
					Data[d].Out.position.x=l-sx;
					Data[d].Out.position.y=y-sy;
					break;
				}
			}
		}	}
	void BitmapFontResource::Print_Test(int x, int y, int w, string text, EPrintData *Data, int DataLen, TextAlignment Align) {
		if(text=="") {
			int d;
			int xpos=0;
			if(Align==TEXTALIGN_CENTER)
				xpos=w/2;
			else if(Align==TEXTALIGN_RIGHT)
				xpos=w;

			for(d=0;d<DataLen;d++) {
				switch(Data[d].Type) {
				case EMT_PositionDetect:
					Data[d].Out.position.x=xpos;
					Data[d].Out.position.y=0;
					break;
				case EMT_Spacing:
					Data[d].Out.position.x=xpos;
					Data[d].Out.position.y=0;
					break;
				}
			}
			return;
		}

		int i,j,d;
		int sx=x;
		int sy=y;
		int datc=0;
		int l=x;
		int lstart=0,lword=0;
		int llen=0;//line length
		int lww=0;
		bool nowrap=false;

		if(w<0) { w=-w; nowrap=true; }
		bool done;
		int fh=FontHeight();
		int *data=(int*)Data;
		bool nextline=false;
		struct {int x;EPrintData*data;} chardetectxs[MAX_CHAR_DETECTS];
		int cchardetectxs=0;

		for(d=0;d<DataLen;d++) {
			switch(Data[d].Type) {
			case EMT_Wrap:
				nowrap=!Data[d].In.value;
				break;
			case EMT_CharacterDetect:
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
		case TEXTALIGN_LEFT:
			l=x;
			break;
		case TEXTALIGN_CENTER:
			l=x+w/2;
			break;
		case TEXTALIGN_RIGHT:
			l=x+w;
			break;
		}
		for(i=0;text[i];i++) {
			for(d=0;d<DataLen;d++) {
				if(Data[d].CharPosition==i) {
					switch(Data[d].Type) {
					case EMT_Spacing:
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
				llen+=Characters[(unsigned char)text[i]]->getWidth()+Seperator;

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
				case TEXTALIGN_LEFT:
					l=x;
					break;
				case TEXTALIGN_CENTER:
					l=x+(w-lww)/2;
					break;
				case TEXTALIGN_RIGHT:
					l=x+(w-lww);
					break;
				}

				for(j=lstart;j<lword+1;j++) {
					done=0;

					for(d=0;d<DataLen;d++) {
						if(Data[d].CharPosition==j) {
							switch(Data[d].Type) {
							case EMT_Spacing:
								Data[d].Out.position.x=l-sx;
								Data[d].Out.position.y=y-sy;

								l+=Data[d].In.position.x-Seperator; 
								y+=Data[d].In.position.y; 
								break;
							case EMT_PositionDetect:
								Data[d].Out.position.x=l-sx;
								Data[d].Out.position.y=y-sy;
								break;
							case EMT_Color:

								break;
							case EMT_ShadowColor:

								break;
							}
						}
					}
					int dist=0;
					if(text[j]=='\t') {
						ImageResource *img=Characters[(unsigned char)' '];
						int i;
						for(i=0;i<Tabsize;i++) {
							dist=img->getWidth()+Seperator;
						}
					}
					if(text[j]!='\r') {
						ImageResource *img=Characters[(unsigned char)text[j]];
						dist+=img->getWidth()+Seperator;
					}
					for(d=0;d<cchardetectxs;d++) {
						if(chardetectxs[d].x<(l-sx)+dist/2 && j<chardetectxs[d].data->Out.value) {
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
					y+=fh*VerticalSpacing;
				nextline=false;
			}
		}
		for(d=0;d<DataLen;d++) {
			if(Data[d].CharPosition==i) {
				switch(Data[d].Type) {
				case EMT_Spacing:
					Data[d].Out.position.x=l-sx;
					Data[d].Out.position.y=y-sy;

					l+=Data[d].In.position.x-Seperator; 
					y+=Data[d].In.position.y; 
					break;
				case EMT_PositionDetect:
					Data[d].Out.position.x=l-sx;
					Data[d].Out.position.y=y-sy;
					break;
				}
			}
		}	
	}
} }
