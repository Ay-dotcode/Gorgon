#include "GraphicLayers.h"
#pragma warning(disable:4244)

namespace gge {
	RGBfloat CurrentLayerColor;
	int trX,trY;
	int scX,scY,scW,scH;

	Basic2DLayer::Basic2DLayer(int X,int Y,int W,int H) : LayerBase() {
		this->X=X;
		this->Y=Y;
		this->W=W;
		this->H=H;
		isVisible=true;

		EnableClipping=false;
	}

	Basic2DLayer::Basic2DLayer(gge::Rectangle r) : LayerBase() {
		this->X=r.Left;
		this->Y=r.Top;
		this->W=r.Width;
		this->H=r.Height;
		isVisible=true;

		EnableClipping=false;
	}
	void Basic2DLayer::Draw(gge::GLTexture *Image, int X1, int Y1, int X2, int Y2, int X3, int Y3, int X4, int Y4) {
		BasicSurface *surface=Surfaces.Add();

		surface->setTexture(Image);
		surface->VertexCoords[0].x=X1;
		surface->VertexCoords[0].y=Y1;

		surface->VertexCoords[1].x=X2;
		surface->VertexCoords[1].y=Y2;

		surface->VertexCoords[2].x=X3;
		surface->VertexCoords[2].y=Y3;

		surface->VertexCoords[3].x=X4;
		surface->VertexCoords[3].y=Y4;
	}

	void Basic2DLayer::DrawTiled(GLTexture *Image,int X,int Y,int W,int H) {
		///*Handles special case where size is 2^n
		if(sl2(Image->H)==Image->H) {
			if(sl2(Image->W)==Image->W) { //both axis are size of 2^n
				BasicSurface *surface=Surfaces.Add();

				surface->setTexture(Image);
				surface->CreateTextureCoords();

				surface->TextureCoords[0].s=0;
				surface->TextureCoords[0].t=0;
				surface->VertexCoords[0].x=X;
				surface->VertexCoords[0].y=Y;

				surface->TextureCoords[1].s=(float)W/Image->W;
				surface->TextureCoords[1].t=0;
				surface->VertexCoords[1].x=X+W;
				surface->VertexCoords[1].y=Y;

				surface->TextureCoords[2].s=(float)W/Image->W;
				surface->TextureCoords[2].t=(float)H/Image->H;
				surface->VertexCoords[2].x=X+W;
				surface->VertexCoords[2].y=Y+H;
	 
				surface->TextureCoords[3].s=0;
				surface->TextureCoords[3].t=(float)H/Image->H;
				surface->VertexCoords[3].x=X;
				surface->VertexCoords[3].y=Y+H;
			} else { //only height is 2^n
				//we leave at least a portion of an image to the last part
				//so that there will be less controls
				int x=X,tx=X+W-Image->W;
				//draw whole parts where image is texture repeated in
				//y axis, we only need to draw multiple images for x axis
				for(x=X;x<tx;x+=Image->W) { 
					BasicSurface *surface=Surfaces.Add();

					surface->setTexture(Image);
					surface->CreateTextureCoords();

					surface->TextureCoords[0].s=0;
					surface->TextureCoords[0].t=0;
					surface->VertexCoords[0].x=x;
					surface->VertexCoords[0].y=Y;

					surface->TextureCoords[1].s=Image->ImageCoord[1].s;
					surface->TextureCoords[1].t=0;
					surface->VertexCoords[1].x=x+Image->W;
					surface->VertexCoords[1].y=Y;

					surface->TextureCoords[2].s=Image->ImageCoord[2].s;
					surface->TextureCoords[2].t=(float)H/Image->H;
					surface->VertexCoords[2].x=x+Image->W;
					surface->VertexCoords[2].y=Y+H;
		 
					surface->TextureCoords[3].s=0;
					surface->TextureCoords[3].t=(float)H/Image->H;
					surface->VertexCoords[3].x=x;
					surface->VertexCoords[3].y=Y+H;
				}

				//last image (might be partial)
				BasicSurface *surface=Surfaces.Add();

				surface->setTexture(Image);
				surface->CreateTextureCoords();

				surface->TextureCoords[0].s=0;
				surface->TextureCoords[0].t=0;
				surface->VertexCoords[0].x=x;
				surface->VertexCoords[0].y=Y;

				surface->TextureCoords[1].s=Image->ImageCoord[1].s*((float)(W-(x-X))/Image->W);
				surface->TextureCoords[1].t=0;
				surface->VertexCoords[1].x=X+W;
				surface->VertexCoords[1].y=Y;

				surface->TextureCoords[2].s=Image->ImageCoord[2].s*((float)(W-(x-X))/Image->W);
				surface->TextureCoords[2].t=(float)H/Image->H;
				surface->VertexCoords[2].x=X+W;
				surface->VertexCoords[2].y=Y+H;
	 
				surface->TextureCoords[3].s=0;
				surface->TextureCoords[3].t=(float)H/Image->H;
				surface->VertexCoords[3].x=x;
				surface->VertexCoords[3].y=Y+H;
			}
		} else {
			if(sl2(Image->W) == Image->W) { //only width is 2^n
				//we leave at least a portion of an image to the last part
				//so that there will be less controls
				int y=Y,ty=Y+H-Image->H;
				//draw whole parts where image is texture repeated in
				//x axis, we only need to draw multiple images for y axis
				for(y=Y;y<ty;y+=Image->H) {
					BasicSurface *surface=Surfaces.Add();

					surface->setTexture(Image);
					surface->CreateTextureCoords();

					surface->TextureCoords[0].s=0;
					surface->TextureCoords[0].t=0;
					surface->VertexCoords[0].x=X;
					surface->VertexCoords[0].y=y;

					surface->TextureCoords[1].s=(float)W/Image->W;
					surface->TextureCoords[1].t=0;
					surface->VertexCoords[1].x=X+W;
					surface->VertexCoords[1].y=y;

					surface->TextureCoords[2].s=(float)W/Image->W;
					surface->TextureCoords[2].t=Image->ImageCoord[2].t;
					surface->VertexCoords[2].x=X+W;
					surface->VertexCoords[2].y=y+Image->H;
		 
					surface->TextureCoords[3].s=0;
					surface->TextureCoords[3].t=Image->ImageCoord[3].t;
					surface->VertexCoords[3].x=X;
					surface->VertexCoords[3].y=y+Image->H;
				}

				//last image (might be partial)
				BasicSurface *surface=Surfaces.Add();

				surface->setTexture(Image);
				surface->CreateTextureCoords();

				surface->TextureCoords[0].s=0;
				surface->TextureCoords[0].t=0;
				surface->VertexCoords[0].x=X;
				surface->VertexCoords[0].y=y;

				surface->TextureCoords[1].s=(float)W/Image->W;
				surface->TextureCoords[1].t=0;
				surface->VertexCoords[1].x=X+W;
				surface->VertexCoords[1].y=y;

				surface->TextureCoords[2].s=(float)W/Image->W;
				surface->TextureCoords[2].t=Image->ImageCoord[2].t*((float)(H-(y-Y))/Image->H);
				surface->VertexCoords[2].x=X+W;
				surface->VertexCoords[2].y=Y+H;
	 
				surface->TextureCoords[3].s=0;
				surface->TextureCoords[3].t=Image->ImageCoord[3].t*((float)(H-(y-Y))/Image->H);
				surface->VertexCoords[3].x=X;
				surface->VertexCoords[3].y=Y+H;
			} else { //if the image has no 2^n dimensions
				//we move row by row
				int y=Y,ty=Y+H-Image->H;
				for(y=Y;y<ty;y+=Image->H) {
					//this part is x axis
					int cy=y+Image->H;
					int x=X,tx=X+W-Image->W;
					for(x=X;x<tx;x+=Image->W) {
						//whole images
						BasicSurface *surface=Surfaces.Add();

						surface->setTexture(Image);
						surface->CreateTextureCoords();

						surface->TextureCoords[0].s=0;
						surface->TextureCoords[0].t=0;
						surface->VertexCoords[0].x=x;
						surface->VertexCoords[0].y=y;

						surface->TextureCoords[1].s=Image->ImageCoord[1].s;
						surface->TextureCoords[1].t=0;
						surface->VertexCoords[1].x=x+Image->W;
						surface->VertexCoords[1].y=y;

						surface->TextureCoords[2].s=Image->ImageCoord[2].s;
						surface->TextureCoords[2].t=Image->ImageCoord[2].t;
						surface->VertexCoords[2].x=x+Image->W;
						surface->VertexCoords[2].y=cy;
			 
						surface->TextureCoords[3].s=0;
						surface->TextureCoords[3].t=Image->ImageCoord[3].t;
						surface->VertexCoords[3].x=x;
						surface->VertexCoords[3].y=cy;
					}

					//partial image at the end of x axis, only x axis is partial
					BasicSurface *surface=Surfaces.Add();

					surface->setTexture(Image);
					surface->CreateTextureCoords();

					surface->TextureCoords[0].s=0;
					surface->TextureCoords[0].t=0;
					surface->VertexCoords[0].x=x;
					surface->VertexCoords[0].y=y;

					surface->TextureCoords[1].s=Image->ImageCoord[1].s*((float)(W-(x-X))/Image->W);
					surface->TextureCoords[1].t=0;
					surface->VertexCoords[1].x=X+W;
					surface->VertexCoords[1].y=y;

					surface->TextureCoords[2].s=Image->ImageCoord[2].s*((float)(W-(x-X))/Image->W);
					surface->TextureCoords[2].t=Image->ImageCoord[2].t;
					surface->VertexCoords[2].x=X+W;
					surface->VertexCoords[2].y=cy;
		 
					surface->TextureCoords[3].s=0;
					surface->TextureCoords[3].t=Image->ImageCoord[2].t;
					surface->VertexCoords[3].x=x;
					surface->VertexCoords[3].y=cy;
					
				}

				//this part is the last partial row
				//this is the partial texture coordinate
				float pty=Image->ImageCoord[2].t*((float)(H-(y-Y))/Image->H);
				int cy=Y+H;
				int x=X,tx=X+W-Image->W;
				for(x=X;x<tx;x+=Image->W) {
					//partial image in y axis
					BasicSurface *surface=Surfaces.Add();

					surface->setTexture(Image);
					surface->CreateTextureCoords();

					surface->TextureCoords[0].s=0;
					surface->TextureCoords[0].t=0;
					surface->VertexCoords[0].x=x;
					surface->VertexCoords[0].y=y;

					surface->TextureCoords[1].s=Image->ImageCoord[1].s;
					surface->TextureCoords[1].t=0;
					surface->VertexCoords[1].x=x+Image->W;
					surface->VertexCoords[1].y=y;

					surface->TextureCoords[2].s=Image->ImageCoord[2].s;
					surface->TextureCoords[2].t=pty;
					surface->VertexCoords[2].x=x+Image->W;
					surface->VertexCoords[2].y=cy;
		 
					surface->TextureCoords[3].s=0;
					surface->TextureCoords[3].t=pty;
					surface->VertexCoords[3].x=x;
					surface->VertexCoords[3].y=cy;
				}

				//partial image at the end of x axis at last row, both axis are partial
				BasicSurface *surface=Surfaces.Add();

				surface->setTexture(Image);
				surface->CreateTextureCoords();

				surface->TextureCoords[0].s=0;
				surface->TextureCoords[0].t=0;
				surface->VertexCoords[0].x=x;
				surface->VertexCoords[0].y=y;

				surface->TextureCoords[1].s=Image->ImageCoord[1].s*((float)(W-(x-X))/Image->W);
				surface->TextureCoords[1].t=0;
				surface->VertexCoords[1].x=X+W;
				surface->VertexCoords[1].y=y;

				surface->TextureCoords[2].s=Image->ImageCoord[2].s*((float)(W-(x-X))/Image->W);
				surface->TextureCoords[2].t=Image->ImageCoord[2].t*((float)(H-(y-Y))/Image->H);
				surface->VertexCoords[2].x=X+W;
				surface->VertexCoords[2].y=Y+H;
	 
				surface->TextureCoords[3].s=0;
				surface->TextureCoords[3].t=Image->ImageCoord[2].t*((float)(H-(y-Y))/Image->H);
				surface->VertexCoords[3].x=x;
				surface->VertexCoords[3].y=Y+H;

			}
		}
	}

	void Basic2DLayer::DrawHTiled(GLTexture *Image,int X,int Y,int W,int H) {
		if(sl2(Image->W)==Image->W) { //both x axis is size of 2^n
			//stretch and repeat as we need
			BasicSurface *surface=Surfaces.Add();

			surface->setTexture(Image);
			surface->CreateTextureCoords();

			surface->TextureCoords[0].s=0;
			surface->TextureCoords[0].t=0;
			surface->VertexCoords[0].x=X;
			surface->VertexCoords[0].y=Y;

			surface->TextureCoords[1].s=(float)W/Image->W;
			surface->TextureCoords[1].t=0;
			surface->VertexCoords[1].x=X+W;
			surface->VertexCoords[1].y=Y;

			surface->TextureCoords[2].s=(float)W/Image->W;
			surface->TextureCoords[2].t=Image->ImageCoord[2].t;
			surface->VertexCoords[2].x=X+W;
			surface->VertexCoords[2].y=Y+H;
 
			surface->TextureCoords[3].s=0;
			surface->TextureCoords[3].t=Image->ImageCoord[2].t;
			surface->VertexCoords[3].x=X;
			surface->VertexCoords[3].y=Y+H;
		} else {
			//we leave at least a portion of an image to the last part
			//so that there will be less controls
			int x=X,tx=X+W-Image->W;
			//draw whole parts where image is texture stretched in
			//y axis, we only need to draw multiple images for x axis
			for(x=X;x<tx;x+=Image->W) { 
				BasicSurface *surface=Surfaces.Add();

				surface->setTexture(Image);
				surface->CreateTextureCoords();

				surface->TextureCoords[0].s=0;
				surface->TextureCoords[0].t=0;
				surface->VertexCoords[0].x=x;
				surface->VertexCoords[0].y=Y;

				surface->TextureCoords[1].s=Image->ImageCoord[1].s;
				surface->TextureCoords[1].t=0;
				surface->VertexCoords[1].x=x+Image->W;
				surface->VertexCoords[1].y=Y;

				surface->TextureCoords[2].s=Image->ImageCoord[2].s;
				surface->TextureCoords[2].t=Image->ImageCoord[2].t;
				surface->VertexCoords[2].x=x+Image->W;
				surface->VertexCoords[2].y=Y+H;
	 
				surface->TextureCoords[3].s=0;
				surface->TextureCoords[3].t=Image->ImageCoord[2].t;
				surface->VertexCoords[3].x=x;
				surface->VertexCoords[3].y=Y+H;
			}

			//last image (might be partial)
			BasicSurface *surface=Surfaces.Add();

			surface->setTexture(Image);
			surface->CreateTextureCoords();

			surface->TextureCoords[0].s=0;
			surface->TextureCoords[0].t=0;
			surface->VertexCoords[0].x=x;
			surface->VertexCoords[0].y=Y;

			surface->TextureCoords[1].s=Image->ImageCoord[1].s*((float)(W-(x-X))/Image->W);
			surface->TextureCoords[1].t=0;
			surface->VertexCoords[1].x=X+W;
			surface->VertexCoords[1].y=Y;

			surface->TextureCoords[2].s=Image->ImageCoord[2].s*((float)(W-(x-X))/Image->W);
			surface->TextureCoords[2].t=Image->ImageCoord[2].t;
			surface->VertexCoords[2].x=X+W;
			surface->VertexCoords[2].y=Y+H;
 
			surface->TextureCoords[3].s=0;
			surface->TextureCoords[3].t=Image->ImageCoord[2].t;
			surface->VertexCoords[3].x=x;
			surface->VertexCoords[3].y=Y+H;	
		}
	}

	void Basic2DLayer::DrawVTiled(GLTexture *Image,int X,int Y,int W,int H) {
		if(sl2(Image->H)==Image->H) { //both x axis is size of 2^n
			//stretch and repeat as we need
			BasicSurface *surface=Surfaces.Add();

			surface->setTexture(Image);
			surface->CreateTextureCoords();

			surface->TextureCoords[0].s=0;
			surface->TextureCoords[0].t=0;
			surface->VertexCoords[0].x=X;
			surface->VertexCoords[0].y=Y;

			surface->TextureCoords[1].s=Image->ImageCoord[1].s;
			surface->TextureCoords[1].t=0;
			surface->VertexCoords[1].x=X+W;
			surface->VertexCoords[1].y=Y;

			surface->TextureCoords[2].s=Image->ImageCoord[2].s;
			surface->TextureCoords[2].t=(float)H/Image->H;
			surface->VertexCoords[2].x=X+W;
			surface->VertexCoords[2].y=Y+H;
 
			surface->TextureCoords[3].s=0;
			surface->TextureCoords[3].t=(float)H/Image->H;
			surface->VertexCoords[3].x=X;
			surface->VertexCoords[3].y=Y+H;
		} else {
				//we leave at least a portion of an image to the last part
				//so that there will be less controls
				int y=Y,ty=Y+H-Image->H;
				//draw whole parts where image is texture repeated in
				//x axis, we only need to draw multiple images for y axis
				for(y=Y;y<ty;y+=Image->H) {
					BasicSurface *surface=Surfaces.Add();

					surface->setTexture(Image);
					surface->CreateTextureCoords();

					surface->TextureCoords[0].s=0;
					surface->TextureCoords[0].t=0;
					surface->VertexCoords[0].x=X;
					surface->VertexCoords[0].y=y;

					surface->TextureCoords[1].s=Image->ImageCoord[1].s;
					surface->TextureCoords[1].t=0;
					surface->VertexCoords[1].x=X+W;
					surface->VertexCoords[1].y=y;

					surface->TextureCoords[2].s=Image->ImageCoord[2].s;
					surface->TextureCoords[2].t=Image->ImageCoord[2].t;
					surface->VertexCoords[2].x=X+W;
					surface->VertexCoords[2].y=y+Image->H;
		 
					surface->TextureCoords[3].s=0;
					surface->TextureCoords[3].t=Image->ImageCoord[3].t;
					surface->VertexCoords[3].x=X;
					surface->VertexCoords[3].y=y+Image->H;
				}

				//last image (might be partial)
				BasicSurface *surface=Surfaces.Add();

				surface->setTexture(Image);
				surface->CreateTextureCoords();

				surface->TextureCoords[0].s=0;
				surface->TextureCoords[0].t=0;
				surface->VertexCoords[0].x=X;
				surface->VertexCoords[0].y=y;

				surface->TextureCoords[1].s=Image->ImageCoord[1].s;
				surface->TextureCoords[1].t=0;
				surface->VertexCoords[1].x=X+W;
				surface->VertexCoords[1].y=y;

				surface->TextureCoords[2].s=Image->ImageCoord[2].s;
				surface->TextureCoords[2].t=Image->ImageCoord[2].t*((float)(H-(y-Y))/Image->H);
				surface->VertexCoords[2].x=X+W;
				surface->VertexCoords[2].y=Y+H;
	 
				surface->TextureCoords[3].s=0;
				surface->TextureCoords[3].t=Image->ImageCoord[3].t*((float)(H-(y-Y))/Image->H);
				surface->VertexCoords[3].x=X;
				surface->VertexCoords[3].y=Y+H;
		}
	}

	void Basic2DLayer::Render() {
		int pscX,pscY,pscW,pscH;
		if(!isVisible) return;
		glPushAttrib(GL_SCISSOR_BIT);

		glPushMatrix();
		glTranslatef(X, Y, 0);
		trX+=X;
		trY+=Y;

		if(EnableClipping) {
			pscX=scX;
			pscY=scY;
			pscW=scW;
			pscH=scH;

			int r=scX+scW;
			int b=scY+scH;

			glEnable(GL_SCISSOR_TEST);
			if(trX>scX)
				scX=trX;
			if(trY>scY)
				scY=trY;
			if(trY+H<b)
				b=(H+trY);
			if(trX+W<r)
				r=(W+trX);

			scW=r-scX;
			scH=b-scY;

			if(scH<=0 || scW<=0) {
				scX=pscX;
				scY=pscY;
				scW=pscW;
				scH=pscH;
			}

			glScissor(scX, (ScreenSize.y-scY)-scH, scW, scH);
		}

		int i;
		for(i=0;i<Surfaces.getCount();i++) {
			BasicSurface *surface=Surfaces[i];
			glBindTexture(GL_TEXTURE_2D, surface->getTexture()->ID);
			glBegin(GL_QUADS);
				glTexCoord2fv(surface->TextureCoords[0].vect);
				glVertex3fv(surface->VertexCoords[0].vect);
				glTexCoord2fv(surface->TextureCoords[1].vect);
				glVertex3fv(surface->VertexCoords[1].vect);
				glTexCoord2fv(surface->TextureCoords[2].vect);
				glVertex3fv(surface->VertexCoords[2].vect);
				glTexCoord2fv(surface->TextureCoords[3].vect);
				glVertex3fv(surface->VertexCoords[3].vect);
			glEnd();
		}

		LayerBase *layer;
		LinkedListOrderedIterator<LayerBase> it=SubLayers.GetReverseOrderedIterator();
		while(layer=it) {
			layer->Render();
		}

		glPopMatrix();
		trX-=X;
		trY-=Y;
		
		if(EnableClipping) {
			scX=pscX;
			scY=pscY;
			scW=pscW;
			scH=pscH;
		}

		glPopAttrib();
	}

	Colorizable2DLayer::Colorizable2DLayer(int X,int Y,int W,int H) : LayerBase() {
		Ambient=RGBint(0xffffffff);
		this->X=X;
		this->Y=Y;
		this->W=W;
		this->H=H;

		isVisible=true;
		EnableClipping=false;
	}
	Colorizable2DLayer::Colorizable2DLayer(gge::Rectangle r) : LayerBase() {
		Ambient=RGBint(0xffffffff);
		this->X=r.Left;
		this->Y=r.Top;
		this->W=r.Width;
		this->H=r.Height;

		isVisible=true;
		EnableClipping=false;
	}
	void Colorizable2DLayer::Render() {
		int pscX,pscY,pscW,pscH;

		if(!isVisible) return;
		glPushAttrib(GL_SCISSOR_BIT);


		glPushMatrix();
		glTranslatef(X, Y, 0);
		trX+=X;
		trY+=Y;

		if(EnableClipping) {
			pscX=scX;
			pscY=scY;
			pscW=scW;
			pscH=scH;

			int r=scX+scW;
			int b=scY+scH;

			glEnable(GL_SCISSOR_TEST);
			if(trX>scX)
				scX=trX;
			if(trY>scY)
				scY=trY;
			if(trY+H<b)
				b=(H+trY);
			if(trX+W<r)
				r=(W+trX);

			scW=r-scX;
			scH=b-scY;

			if(scH<=0 || scW<=0) {
				scX=pscX;
				scY=pscY;
				scW=pscW;
				scH=pscH;
			}

			glScissor(scX, (ScreenSize.y-scY)-scH, scW, scH);
		}

		RGBfloat prevcolor=CurrentLayerColor;
		CurrentLayerColor.a*=(float)Ambient.a/255;
		CurrentLayerColor.r*=(float)Ambient.r/255;
		CurrentLayerColor.g*=(float)Ambient.g/255;
		CurrentLayerColor.b*=(float)Ambient.b/255;

		int i;
		for(i=0;i<Surfaces.getCount();i++) {
			ColorizableSurface *surface=Surfaces[i];
			glColor4f(surface->Color.r*CurrentLayerColor.r,surface->Color.g*CurrentLayerColor.g,surface->Color.b*CurrentLayerColor.b,surface->Color.a*CurrentLayerColor.a);
			glBindTexture(GL_TEXTURE_2D, surface->getTexture()->ID);
			glBegin(GL_QUADS);
				glTexCoord2fv(surface->TextureCoords[0].vect);
				glVertex3fv(surface->VertexCoords[0].vect);
				glTexCoord2fv(surface->TextureCoords[1].vect);
				glVertex3fv(surface->VertexCoords[1].vect);
				glTexCoord2fv(surface->TextureCoords[2].vect);
				glVertex3fv(surface->VertexCoords[2].vect);
				glTexCoord2fv(surface->TextureCoords[3].vect);
				glVertex3fv(surface->VertexCoords[3].vect);
			glEnd();
		}

		glColor4fv(CurrentLayerColor.vect);
		LayerBase *layer;
		LinkedListOrderedIterator<LayerBase> it=SubLayers.GetReverseOrderedIterator();
		while(layer=it) {
			layer->Render();
		}

		CurrentLayerColor=prevcolor;
		glColor4fv(prevcolor.vect);

		glPopMatrix();
		trX-=X;
		trY-=Y;
		
		if(EnableClipping) {
			scX=pscX;
			scY=pscY;
			scW=pscW;
			scH=pscH;
		}

		glPopAttrib();

	}

	void Colorizable2DLayer::Draw(gge::GLTexture *Image, int X1, int Y1, int X2, int Y2, int X3, int Y3, int X4, int Y4, RGBint color) {
		ColorizableSurface *surface=Surfaces.Add();

		surface->setTexture(Image);
		surface->VertexCoords[0].x=X1;
		surface->VertexCoords[0].y=Y1;

		surface->VertexCoords[1].x=X2;
		surface->VertexCoords[1].y=Y2;

		surface->VertexCoords[2].x=X3;
		surface->VertexCoords[2].y=Y3;

		surface->VertexCoords[3].x=X4;
		surface->VertexCoords[3].y=Y4;
		surface->Color=ToRGBfloat(color);
	}

	void Colorizable2DLayer::Draw(gge::GLTexture *Image, int X1, int Y1, int X2, int Y2, int X3, int Y3, int X4, int Y4) {
		ColorizableSurface *surface=Surfaces.Add();

		surface->setTexture(Image);
		surface->VertexCoords[0].x=X1;
		surface->VertexCoords[0].y=Y1;

		surface->VertexCoords[1].x=X2;
		surface->VertexCoords[1].y=Y2;

		surface->VertexCoords[2].x=X3;
		surface->VertexCoords[2].y=Y3;

		surface->VertexCoords[3].x=X4;
		surface->VertexCoords[3].y=Y4;
		surface->Color.a=surface->Color.r=surface->Color.g=surface->Color.b=1;
	}

	void Colorizable2DLayer::DrawTiled(GLTexture *Image,int X,int Y,int W,int H) {
		///*Handles special case where size is 2^n
		if(sl2(Image->H)==Image->H) {
			if(sl2(Image->W)==Image->W) { //both axis are size of 2^n
				ColorizableSurface *surface=Surfaces.Add();
				surface->Color.a=surface->Color.r=surface->Color.g=surface->Color.b=1;

				surface->setTexture(Image);
				surface->CreateTextureCoords();

				surface->TextureCoords[0].s=0;
				surface->TextureCoords[0].t=0;
				surface->VertexCoords[0].x=X;
				surface->VertexCoords[0].y=Y;

				surface->TextureCoords[1].s=(float)W/Image->W;
				surface->TextureCoords[1].t=0;
				surface->VertexCoords[1].x=X+W;
				surface->VertexCoords[1].y=Y;

				surface->TextureCoords[2].s=(float)W/Image->W;
				surface->TextureCoords[2].t=(float)H/Image->H;
				surface->VertexCoords[2].x=X+W;
				surface->VertexCoords[2].y=Y+H;
	 
				surface->TextureCoords[3].s=0;
				surface->TextureCoords[3].t=(float)H/Image->H;
				surface->VertexCoords[3].x=X;
				surface->VertexCoords[3].y=Y+H;
			} else { //only height is 2^n
				//we leave at least a portion of an image to the last part
				//so that there will be less controls
				int x=X,tx=X+W-Image->W;
				//draw whole parts where image is texture repeated in
				//y axis, we only need to draw multiple images for x axis
				for(x=X;x<tx;x+=Image->W) { 
					ColorizableSurface *surface=Surfaces.Add();
					surface->Color.a=surface->Color.r=surface->Color.g=surface->Color.b=1;

					surface->setTexture(Image);
					surface->CreateTextureCoords();

					surface->TextureCoords[0].s=0;
					surface->TextureCoords[0].t=0;
					surface->VertexCoords[0].x=x;
					surface->VertexCoords[0].y=Y;

					surface->TextureCoords[1].s=Image->ImageCoord[1].s;
					surface->TextureCoords[1].t=0;
					surface->VertexCoords[1].x=x+Image->W;
					surface->VertexCoords[1].y=Y;

					surface->TextureCoords[2].s=Image->ImageCoord[2].s;
					surface->TextureCoords[2].t=(float)H/Image->H;
					surface->VertexCoords[2].x=x+Image->W;
					surface->VertexCoords[2].y=Y+H;
		 
					surface->TextureCoords[3].s=0;
					surface->TextureCoords[3].t=(float)H/Image->H;
					surface->VertexCoords[3].x=x;
					surface->VertexCoords[3].y=Y+H;
				}

				//last image (might be partial)
				ColorizableSurface *surface=Surfaces.Add();
				surface->Color.a=surface->Color.r=surface->Color.g=surface->Color.b=1;

				surface->setTexture(Image);
				surface->CreateTextureCoords();

				surface->TextureCoords[0].s=0;
				surface->TextureCoords[0].t=0;
				surface->VertexCoords[0].x=x;
				surface->VertexCoords[0].y=Y;

				surface->TextureCoords[1].s=Image->ImageCoord[1].s*((float)(W-(x-X))/Image->W);
				surface->TextureCoords[1].t=0;
				surface->VertexCoords[1].x=X+W;
				surface->VertexCoords[1].y=Y;

				surface->TextureCoords[2].s=Image->ImageCoord[2].s*((float)(W-(x-X))/Image->W);
				surface->TextureCoords[2].t=(float)H/Image->H;
				surface->VertexCoords[2].x=X+W;
				surface->VertexCoords[2].y=Y+H;
	 
				surface->TextureCoords[3].s=0;
				surface->TextureCoords[3].t=(float)H/Image->H;
				surface->VertexCoords[3].x=x;
				surface->VertexCoords[3].y=Y+H;
			}
		} else {
			if(sl2(Image->W) == Image->W) { //only width is 2^n
				//we leave at least a portion of an image to the last part
				//so that there will be less controls
				int y=Y,ty=Y+H-Image->H;
				//draw whole parts where image is texture repeated in
				//x axis, we only need to draw multiple images for y axis
				for(y=Y;y<ty;y+=Image->H) {
					ColorizableSurface *surface=Surfaces.Add();
					surface->Color.a=surface->Color.r=surface->Color.g=surface->Color.b=1;

					surface->setTexture(Image);
					surface->CreateTextureCoords();

					surface->TextureCoords[0].s=0;
					surface->TextureCoords[0].t=0;
					surface->VertexCoords[0].x=X;
					surface->VertexCoords[0].y=y;

					surface->TextureCoords[1].s=(float)W/Image->W;
					surface->TextureCoords[1].t=0;
					surface->VertexCoords[1].x=X+W;
					surface->VertexCoords[1].y=y;

					surface->TextureCoords[2].s=(float)W/Image->W;
					surface->TextureCoords[2].t=Image->ImageCoord[2].t;
					surface->VertexCoords[2].x=X+W;
					surface->VertexCoords[2].y=y+Image->H;
		 
					surface->TextureCoords[3].s=0;
					surface->TextureCoords[3].t=Image->ImageCoord[3].t;
					surface->VertexCoords[3].x=X;
					surface->VertexCoords[3].y=y+Image->H;
				}

				//last image (might be partial)
				ColorizableSurface *surface=Surfaces.Add();
				surface->Color.a=surface->Color.r=surface->Color.g=surface->Color.b=1;

				surface->setTexture(Image);
				surface->CreateTextureCoords();

				surface->TextureCoords[0].s=0;
				surface->TextureCoords[0].t=0;
				surface->VertexCoords[0].x=X;
				surface->VertexCoords[0].y=y;

				surface->TextureCoords[1].s=(float)W/Image->W;
				surface->TextureCoords[1].t=0;
				surface->VertexCoords[1].x=X+W;
				surface->VertexCoords[1].y=y;

				surface->TextureCoords[2].s=(float)W/Image->W;
				surface->TextureCoords[2].t=Image->ImageCoord[2].t*((float)(H-(y-Y))/Image->H);
				surface->VertexCoords[2].x=X+W;
				surface->VertexCoords[2].y=Y+H;
	 
				surface->TextureCoords[3].s=0;
				surface->TextureCoords[3].t=Image->ImageCoord[3].t*((float)(H-(y-Y))/Image->H);
				surface->VertexCoords[3].x=X;
				surface->VertexCoords[3].y=Y+H;
			} else { //if the image has no 2^n dimensions
				//we move row by row
				int y=Y,ty=Y+H-Image->H;
				for(y=Y;y<ty;y+=Image->H) {
					//this part is x axis
					int cy=y+Image->H;
					int x=X,tx=X+W-Image->W;
					for(x=X;x<tx;x+=Image->W) {
						//whole images
						ColorizableSurface *surface=Surfaces.Add();
						surface->Color.a=surface->Color.r=surface->Color.g=surface->Color.b=1;

						surface->setTexture(Image);
						surface->CreateTextureCoords();

						surface->TextureCoords[0].s=0;
						surface->TextureCoords[0].t=0;
						surface->VertexCoords[0].x=x;
						surface->VertexCoords[0].y=y;

						surface->TextureCoords[1].s=Image->ImageCoord[1].s;
						surface->TextureCoords[1].t=0;
						surface->VertexCoords[1].x=x+Image->W;
						surface->VertexCoords[1].y=y;

						surface->TextureCoords[2].s=Image->ImageCoord[2].s;
						surface->TextureCoords[2].t=Image->ImageCoord[2].t;
						surface->VertexCoords[2].x=x+Image->W;
						surface->VertexCoords[2].y=cy;
			 
						surface->TextureCoords[3].s=0;
						surface->TextureCoords[3].t=Image->ImageCoord[3].t;
						surface->VertexCoords[3].x=x;
						surface->VertexCoords[3].y=cy;
					}

					//partial image at the end of x axis, only x axis is partial
					ColorizableSurface *surface=Surfaces.Add();
					surface->Color.a=surface->Color.r=surface->Color.g=surface->Color.b=1;

					surface->setTexture(Image);
					surface->CreateTextureCoords();

					surface->TextureCoords[0].s=0;
					surface->TextureCoords[0].t=0;
					surface->VertexCoords[0].x=x;
					surface->VertexCoords[0].y=y;

					surface->TextureCoords[1].s=Image->ImageCoord[1].s*((float)(W-(x-X))/Image->W);
					surface->TextureCoords[1].t=0;
					surface->VertexCoords[1].x=X+W;
					surface->VertexCoords[1].y=y;

					surface->TextureCoords[2].s=Image->ImageCoord[2].s*((float)(W-(x-X))/Image->W);
					surface->TextureCoords[2].t=Image->ImageCoord[2].t;
					surface->VertexCoords[2].x=X+W;
					surface->VertexCoords[2].y=cy;
		 
					surface->TextureCoords[3].s=0;
					surface->TextureCoords[3].t=Image->ImageCoord[2].t;
					surface->VertexCoords[3].x=x;
					surface->VertexCoords[3].y=cy;
					
				}

				//this part is the last partial row
				//this is the partial texture coordinate
				float pty=Image->ImageCoord[2].t*((float)(H-(y-Y))/Image->H);
				int cy=Y+H;
				int x=X,tx=X+W-Image->W;
				for(x=X;x<tx;x+=Image->W) {
					//partial image in y axis
					ColorizableSurface *surface=Surfaces.Add();
					surface->Color.a=surface->Color.r=surface->Color.g=surface->Color.b=1;

					surface->setTexture(Image);
					surface->CreateTextureCoords();

					surface->TextureCoords[0].s=0;
					surface->TextureCoords[0].t=0;
					surface->VertexCoords[0].x=x;
					surface->VertexCoords[0].y=y;

					surface->TextureCoords[1].s=Image->ImageCoord[1].s;
					surface->TextureCoords[1].t=0;
					surface->VertexCoords[1].x=x+Image->W;
					surface->VertexCoords[1].y=y;

					surface->TextureCoords[2].s=Image->ImageCoord[2].s;
					surface->TextureCoords[2].t=pty;
					surface->VertexCoords[2].x=x+Image->W;
					surface->VertexCoords[2].y=cy;
		 
					surface->TextureCoords[3].s=0;
					surface->TextureCoords[3].t=pty;
					surface->VertexCoords[3].x=x;
					surface->VertexCoords[3].y=cy;
				}

				//partial image at the end of x axis at last row, both axis are partial
				ColorizableSurface *surface=Surfaces.Add();
				surface->Color.a=surface->Color.r=surface->Color.g=surface->Color.b=1;

				surface->setTexture(Image);
				surface->CreateTextureCoords();

				surface->TextureCoords[0].s=0;
				surface->TextureCoords[0].t=0;
				surface->VertexCoords[0].x=x;
				surface->VertexCoords[0].y=y;

				surface->TextureCoords[1].s=Image->ImageCoord[1].s*((float)(W-(x-X))/Image->W);
				surface->TextureCoords[1].t=0;
				surface->VertexCoords[1].x=X+W;
				surface->VertexCoords[1].y=y;

				surface->TextureCoords[2].s=Image->ImageCoord[2].s*((float)(W-(x-X))/Image->W);
				surface->TextureCoords[2].t=Image->ImageCoord[2].t*((float)(H-(y-Y))/Image->H);
				surface->VertexCoords[2].x=X+W;
				surface->VertexCoords[2].y=Y+H;
	 
				surface->TextureCoords[3].s=0;
				surface->TextureCoords[3].t=Image->ImageCoord[2].t*((float)(H-(y-Y))/Image->H);
				surface->VertexCoords[3].x=x;
				surface->VertexCoords[3].y=Y+H;

			}
		}
	}

	void Colorizable2DLayer::DrawHTiled(GLTexture *Image,int X,int Y,int W,int H) {
		if(sl2(Image->W)==Image->W) { //both x axis is size of 2^n
			//stretch and repeat as we need
			ColorizableSurface *surface=Surfaces.Add();
			surface->Color.a=surface->Color.r=surface->Color.g=surface->Color.b=1;

			surface->setTexture(Image);
			surface->CreateTextureCoords();

			surface->TextureCoords[0].s=0;
			surface->TextureCoords[0].t=0;
			surface->VertexCoords[0].x=X;
			surface->VertexCoords[0].y=Y;

			surface->TextureCoords[1].s=(float)W/Image->W;
			surface->TextureCoords[1].t=0;
			surface->VertexCoords[1].x=X+W;
			surface->VertexCoords[1].y=Y;

			surface->TextureCoords[2].s=(float)W/Image->W;
			surface->TextureCoords[2].t=Image->ImageCoord[2].t;
			surface->VertexCoords[2].x=X+W;
			surface->VertexCoords[2].y=Y+H;
 
			surface->TextureCoords[3].s=0;
			surface->TextureCoords[3].t=Image->ImageCoord[2].t;
			surface->VertexCoords[3].x=X;
			surface->VertexCoords[3].y=Y+H;
		} else {
			//we leave at least a portion of an image to the last part
			//so that there will be less controls
			int x=X,tx=X+W-Image->W;
			//draw whole parts where image is texture stretched in
			//y axis, we only need to draw multiple images for x axis
			for(x=X;x<tx;x+=Image->W) { 
				ColorizableSurface *surface=Surfaces.Add();
				surface->Color.a=surface->Color.r=surface->Color.g=surface->Color.b=1;

				surface->setTexture(Image);
				surface->CreateTextureCoords();

				surface->TextureCoords[0].s=0;
				surface->TextureCoords[0].t=0;
				surface->VertexCoords[0].x=x;
				surface->VertexCoords[0].y=Y;

				surface->TextureCoords[1].s=Image->ImageCoord[1].s;
				surface->TextureCoords[1].t=0;
				surface->VertexCoords[1].x=x+Image->W;
				surface->VertexCoords[1].y=Y;

				surface->TextureCoords[2].s=Image->ImageCoord[2].s;
				surface->TextureCoords[2].t=Image->ImageCoord[2].t;
				surface->VertexCoords[2].x=x+Image->W;
				surface->VertexCoords[2].y=Y+H;
	 
				surface->TextureCoords[3].s=0;
				surface->TextureCoords[3].t=Image->ImageCoord[2].t;
				surface->VertexCoords[3].x=x;
				surface->VertexCoords[3].y=Y+H;
			}

			//last image (might be partial)
			ColorizableSurface *surface=Surfaces.Add();
			surface->Color.a=surface->Color.r=surface->Color.g=surface->Color.b=1;

			surface->setTexture(Image);
			surface->CreateTextureCoords();

			surface->TextureCoords[0].s=0;
			surface->TextureCoords[0].t=0;
			surface->VertexCoords[0].x=x;
			surface->VertexCoords[0].y=Y;

			surface->TextureCoords[1].s=Image->ImageCoord[1].s*((float)(W-(x-X))/Image->W);
			surface->TextureCoords[1].t=0;
			surface->VertexCoords[1].x=X+W;
			surface->VertexCoords[1].y=Y;

			surface->TextureCoords[2].s=Image->ImageCoord[2].s*((float)(W-(x-X))/Image->W);
			surface->TextureCoords[2].t=Image->ImageCoord[2].t;
			surface->VertexCoords[2].x=X+W;
			surface->VertexCoords[2].y=Y+H;
 
			surface->TextureCoords[3].s=0;
			surface->TextureCoords[3].t=Image->ImageCoord[2].t;
			surface->VertexCoords[3].x=x;
			surface->VertexCoords[3].y=Y+H;	
		}
	}

	void Colorizable2DLayer::DrawVTiled(GLTexture *Image,int X,int Y,int W,int H) {
		if(sl2(Image->H)==Image->H) { //both x axis is size of 2^n
			//stretch and repeat as we need
			ColorizableSurface *surface=Surfaces.Add();
			surface->Color.a=surface->Color.r=surface->Color.g=surface->Color.b=1;

			surface->setTexture(Image);
			surface->CreateTextureCoords();

			surface->TextureCoords[0].s=0;
			surface->TextureCoords[0].t=0;
			surface->VertexCoords[0].x=X;
			surface->VertexCoords[0].y=Y;

			surface->TextureCoords[1].s=Image->ImageCoord[1].s;
			surface->TextureCoords[1].t=0;
			surface->VertexCoords[1].x=X+W;
			surface->VertexCoords[1].y=Y;

			surface->TextureCoords[2].s=Image->ImageCoord[2].s;
			surface->TextureCoords[2].t=(float)H/Image->H;
			surface->VertexCoords[2].x=X+W;
			surface->VertexCoords[2].y=Y+H;
 
			surface->TextureCoords[3].s=0;
			surface->TextureCoords[3].t=(float)H/Image->H;
			surface->VertexCoords[3].x=X;
			surface->VertexCoords[3].y=Y+H;
		} else {
				//we leave at least a portion of an image to the last part
				//so that there will be less controls
				int y=Y,ty=Y+H-Image->H;
				//draw whole parts where image is texture repeated in
				//x axis, we only need to draw multiple images for y axis
				for(y=Y;y<ty;y+=Image->H) {
					ColorizableSurface *surface=Surfaces.Add();
					surface->Color.a=surface->Color.r=surface->Color.g=surface->Color.b=1;

					surface->setTexture(Image);
					surface->CreateTextureCoords();

					surface->TextureCoords[0].s=0;
					surface->TextureCoords[0].t=0;
					surface->VertexCoords[0].x=X;
					surface->VertexCoords[0].y=y;

					surface->TextureCoords[1].s=Image->ImageCoord[1].s;
					surface->TextureCoords[1].t=0;
					surface->VertexCoords[1].x=X+W;
					surface->VertexCoords[1].y=y;

					surface->TextureCoords[2].s=Image->ImageCoord[2].s;
					surface->TextureCoords[2].t=Image->ImageCoord[2].t;
					surface->VertexCoords[2].x=X+W;
					surface->VertexCoords[2].y=y+Image->H;
		 
					surface->TextureCoords[3].s=0;
					surface->TextureCoords[3].t=Image->ImageCoord[3].t;
					surface->VertexCoords[3].x=X;
					surface->VertexCoords[3].y=y+Image->H;
				}

				//last image (might be partial)
				ColorizableSurface *surface=Surfaces.Add();
				surface->Color.a=surface->Color.r=surface->Color.g=surface->Color.b=1;

				surface->setTexture(Image);
				surface->CreateTextureCoords();

				surface->TextureCoords[0].s=0;
				surface->TextureCoords[0].t=0;
				surface->VertexCoords[0].x=X;
				surface->VertexCoords[0].y=y;

				surface->TextureCoords[1].s=Image->ImageCoord[1].s;
				surface->TextureCoords[1].t=0;
				surface->VertexCoords[1].x=X+W;
				surface->VertexCoords[1].y=y;

				surface->TextureCoords[2].s=Image->ImageCoord[2].s;
				surface->TextureCoords[2].t=Image->ImageCoord[2].t*((float)(H-(y-Y))/Image->H);
				surface->VertexCoords[2].x=X+W;
				surface->VertexCoords[2].y=Y+H;
	 
				surface->TextureCoords[3].s=0;
				surface->TextureCoords[3].t=Image->ImageCoord[3].t*((float)(H-(y-Y))/Image->H);
				surface->VertexCoords[3].x=X;
				surface->VertexCoords[3].y=Y+H;
		}
	}

	void Basic2DRawGraphicsLayer::Draw(Byte *Image, int Width, int Height, gge::ColorMode Mode, int X1, int Y1, int X2, int Y2, int X3, int Y3, int X4, int Y4) {
		RawSurface *surface=Surfaces.Add();

		surface->Data=Image;
		surface->Mode=Mode;
		surface->CreateTextureCoords();
		surface->Width=Width;
		surface->Height=Height;


		surface->TextureCoords[0].s=0;
		surface->TextureCoords[0].t=0;
		surface->TextureCoords[1].s=(float)Width/sl2(Width);
		surface->TextureCoords[1].t=0;
		surface->TextureCoords[2].s=(float)Width/sl2(Width);
		surface->TextureCoords[2].t=(float)Height/sl2(Height);
		surface->TextureCoords[3].s=0;
		surface->TextureCoords[3].t=(float)Height/sl2(Height);

		surface->VertexCoords[0].x=X1;
		surface->VertexCoords[0].y=Y1;

		surface->VertexCoords[1].x=X2;
		surface->VertexCoords[1].y=Y2;

		surface->VertexCoords[2].x=X3;
		surface->VertexCoords[2].y=Y3;

		surface->VertexCoords[3].x=X4;
		surface->VertexCoords[3].y=Y4;
	}
	void Basic2DRawGraphicsLayer::Render() {
		if(!isVisible) return;
		int pscX,pscY,pscW,pscH;

		glPushAttrib(GL_SCISSOR_BIT);


		glPushMatrix();
		glTranslatef(X, Y, 0);
		trX+=X;
		trY+=Y;

		if(EnableClipping) {
			pscX=scX;
			pscY=scY;
			pscW=scW;
			pscH=scH;

			int r=scX+scW;
			int b=scY+scH;

			glEnable(GL_SCISSOR_TEST);
			if(trX>scX)
				scX=trX;
			if(trY>scY)
				scY=trY;
			if(trY+H<b)
				b=(H+trY);
			if(trX+W<r)
				r=(W+trX);

			scW=r-scX;
			scH=b-scY;

			if(scH<=0 || scW<=0) {
				scX=pscX;
				scY=pscY;
				scW=pscW;
				scH=pscH;
			}

			glScissor(scX, (ScreenSize.y-scY)-scH, scW, scH);
		}

		int i;
		for(i=0;i<Surfaces.getCount();i++) {
			RawSurface *surface=Surfaces[i];
			glBindTexture(GL_TEXTURE_2D, NULL);
			SetTexture(surface->Data, surface->Width, surface->Height, surface->Mode);

			glBegin(GL_QUADS);
				glTexCoord2fv(surface->TextureCoords[0].vect);
				glVertex3fv(surface->VertexCoords[0].vect);
				glTexCoord2fv(surface->TextureCoords[1].vect);
				glVertex3fv(surface->VertexCoords[1].vect);
				glTexCoord2fv(surface->TextureCoords[2].vect);
				glVertex3fv(surface->VertexCoords[2].vect);
				glTexCoord2fv(surface->TextureCoords[3].vect);
				glVertex3fv(surface->VertexCoords[3].vect);
			glEnd();
		}

		LayerBase *layer;
		LinkedListOrderedIterator<LayerBase> it=SubLayers.GetReverseOrderedIterator();
		while(layer=it) {
			layer->Render();
		}

		glPopMatrix();
		trX-=X;
		trY-=Y;
		
		if(EnableClipping) {
			scX=pscX;
			scY=pscY;
			scW=pscW;
			scH=pscH;
		}

		glPopAttrib();
	}
	Basic2DRawGraphicsLayer::Basic2DRawGraphicsLayer(int X,int Y,int W,int H) : LayerBase() {
		this->X=X;
		this->Y=Y;
		this->W=W;
		this->H=H;

		isVisible=true;
		EnableClipping=false;
	}
	bool WidgetLayer::PropagateMouseEvent(MouseEventType event, int x, int y, void *data) {
		if( isVisible && ((x>X && y>Y && x<X+W && y<Y+H) || (event&MOUSE_EVENT_UP) || (pressedObject && event&MOUSE_EVENT_MOVE)) ) {
			if(LayerBase::PropagateMouseEvent(event, x, y, data))
				return true;

			if( isVisible && ((x>X && y>Y && x<X+W && y<Y+H) || (event&MOUSE_EVENT_UP) || (pressedObject->parent==this && event&MOUSE_EVENT_MOVE)) )
				return BasicPointerTarget::PropagateMouseEvent(event, x-X, y-Y, data);
			else
				return false;
		}

		return false;
	}
	bool WidgetLayer::PropagateMouseScrollEvent(int amount, MouseEventType event, int x, int y, void *data) {
		if( isVisible && ((x>X && y>Y && x<X+W && y<Y+H)) ) {
			if(LayerBase::PropagateMouseScrollEvent(amount, event, x, y, data))
				return true;

			if( isVisible && ((x>X && y>Y && x<X+W && y<Y+H)) )
				return BasicPointerTarget::PropagateMouseScrollEvent(amount, event, x-X, y-Y, data);
			else
				return false;
		}

		return false;
	}
}
