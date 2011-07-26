#include "GraphicLayers.h"
#pragma warning(disable:4244)



using namespace gge::utils;
using namespace gge::input;
using namespace gge::graphics::system;

namespace gge { namespace graphics {
	RGBfloat CurrentLayerColor;
	Point translate;
	Rectangle scissors;

	void Basic2DLayer::Draw(GLTexture *Image, int X1, int Y1, int X2, int Y2, int X3, int Y3, int X4, int Y4) {
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

	void Basic2DLayer::Draw(GLTexture *Image, int X1, int Y1, int X2, int Y2, int X3, int Y3, int X4, int Y4,  float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) {
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

		surface->CreateTextureCoords();

		float w=Image->ImageCoord[2].s, h=Image->ImageCoord[2].t;
		surface->TextureCoords[0].s=w*S1;
		surface->TextureCoords[0].t=h*U1;

		surface->TextureCoords[1].s=w*S2;
		surface->TextureCoords[1].t=h*U2;

		surface->TextureCoords[2].s=w*S3;
		surface->TextureCoords[2].t=h*U3;

		surface->TextureCoords[3].s=w*S4;
		surface->TextureCoords[3].t=h*U4;
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
		Rectangle psc;
		if(!isVisible) return;
		glPushAttrib(GL_SCISSOR_BIT);

		glPushMatrix();
		glTranslatef(BoundingBox.Left, BoundingBox.Top, 0);
		translate+=BoundingBox.TopLeft();

		if(EnableClipping) {
			psc=scissors;

			int r=scissors.Right();
			int b=scissors.Bottom();

			glEnable(GL_SCISSOR_TEST);
			if(translate.x>scissors.Left)
				scissors.Left=translate.x;
			if(translate.y>scissors.Top)
				scissors.Top=translate.y;
			if(translate.y+BoundingBox.Height()<b)
				b=(translate.y+BoundingBox.Height());
			if(translate.x+BoundingBox.Width()<r)
				r=(translate.x+BoundingBox.Width());

			scissors.SetRight(r);
			scissors.SetBottom(b);

			if(r<=scissors.Left || b<=scissors.Top) {
				return;
			}

			glScissor(scissors.Left, (ScreenSize.Height-scissors.Top)-scissors.Height, scissors.Width, scissors.Height);
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

		for(utils::SortedCollection<LayerBase>::Iterator i=SubLayers.Last(); i.isValid(); i.Previous()) {
			i->Render();
		}

		glPopMatrix();
		translate-=BoundingBox.TopLeft();

		if(EnableClipping) {
			scissors=psc;
		}

		glPopAttrib();
	}


	void Colorizable2DLayer::Draw(GLTexture *Image, int X1, int Y1, int X2, int Y2, int X3, int Y3, int X4, int Y4) {
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

		surface->Color=ToRGBfloat(CurrentColor);
	}

	void Colorizable2DLayer::Draw(GLTexture *Image, int X1, int Y1, int X2, int Y2, int X3, int Y3, int X4, int Y4,  float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) {
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

		surface->CreateTextureCoords();

		float w=Image->ImageCoord[2].s, h=Image->ImageCoord[2].t;
		surface->TextureCoords[0].s=w*S1;
		surface->TextureCoords[0].t=h*U1;

		surface->TextureCoords[1].s=w*S2;
		surface->TextureCoords[1].t=h*U2;

		surface->TextureCoords[2].s=w*S3;
		surface->TextureCoords[2].t=h*U3;

		surface->TextureCoords[3].s=w*S4;
		surface->TextureCoords[3].t=h*U4;

		surface->Color=ToRGBfloat(CurrentColor);
	}

	void Colorizable2DLayer::DrawTiled(GLTexture *Image,int X,int Y,int W,int H) {
		///*Handles special case where size is 2^n
		if(sl2(Image->H)==Image->H) {
			if(sl2(Image->W)==Image->W) { //both axis are size of 2^n
				ColorizableSurface *surface=Surfaces.Add();
				surface->Color=ToRGBfloat(CurrentColor);

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
					surface->Color=ToRGBfloat(CurrentColor);

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
				surface->Color=ToRGBfloat(CurrentColor);

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
					surface->Color=ToRGBfloat(CurrentColor);

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
				surface->Color=ToRGBfloat(CurrentColor);

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
						surface->Color=ToRGBfloat(CurrentColor);

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
					surface->Color=ToRGBfloat(CurrentColor);

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
					surface->Color=ToRGBfloat(CurrentColor);

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
				surface->Color=ToRGBfloat(CurrentColor);

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
			surface->Color=ToRGBfloat(CurrentColor);

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
				surface->Color=ToRGBfloat(CurrentColor);

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
			surface->Color=ToRGBfloat(CurrentColor);

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
			surface->Color=ToRGBfloat(CurrentColor);

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
				surface->Color=ToRGBfloat(CurrentColor);

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
			surface->Color=ToRGBfloat(CurrentColor);

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

	void Colorizable2DLayer::Render() {
		Rectangle psc;
		if(!isVisible) return;
		if(Ambient.a==0) return;
		glPushAttrib(GL_SCISSOR_BIT);

		glPushMatrix();
		glTranslatef(BoundingBox.Left, BoundingBox.Top, 0);
		translate+=BoundingBox.TopLeft();

		if(EnableClipping) {
			psc=scissors;

			int r=scissors.Right();
			int b=scissors.Bottom();

			glEnable(GL_SCISSOR_TEST);
			if(translate.x>scissors.Left)
				scissors.Left=translate.x;
			if(translate.y>scissors.Top)
				scissors.Top=translate.y;
			if(translate.y+BoundingBox.Height()<b)
				b=(translate.y+BoundingBox.Height());
			if(translate.x+BoundingBox.Width()<r)
				r=(translate.x+BoundingBox.Width());

			scissors.SetRight(r);
			scissors.SetBottom(b);

			if(r<=scissors.Left || b<=scissors.Top) {
				return;
			}

			glScissor(scissors.Left, (ScreenSize.Height-scissors.Top)-scissors.Height, scissors.Width, scissors.Height);
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
		for(utils::SortedCollection<LayerBase>::Iterator i=SubLayers.Last(); i.isValid(); i.Previous()) {
			i->Render();
		}


		CurrentLayerColor=prevcolor;
		glColor4fv(prevcolor.vect);


		glPopMatrix();
		translate-=BoundingBox.TopLeft();

		if(EnableClipping) {
			scissors=psc;
		}

		glPopAttrib();

	}

} }
