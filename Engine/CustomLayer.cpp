#include "CustomLayer.h"
#include "Graphics.h"
#include "GGEMain.h"
#include <gl/gl.h>

namespace gge {
	extern RGBfloat CurrentLayerColor;
	extern int trX,trY;
	extern int scX,scY,scW,scH;

	CustomLayer::CustomLayer(CustomRenderer Renderer,int X, int Y, int W, int H) : LayerBase() {
		this->X=X;
		this->Y=Y;
		this->W=W;
		this->H=H;
		this->Renderer=Renderer;

		Xs=-10;
		Xe= 10;
		Ys=-10;
		Ye= 10;
		Zs=-10;
		Ze= 10;
	}

	void CustomLayer::Render() {
		int pscX,pscY,pscW,pscH;
		trX+=X;
		trY+=Y;
		glPushAttrib(GL_SCISSOR_BIT);
		glEnable(GL_SCISSOR_TEST);

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


		glColor4f(1,1,1,1);
		glViewport(X, (Main.H-Y)-H, W, H);
		glClear(GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

		glLoadIdentity();
		glOrtho(Xs, Xe, Ys, Ye, Zs, Ze);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glBindTexture(GL_TEXTURE_2D, -1);
		glDepthFunc(GL_LESS);

		this->Renderer();

		glDepthFunc(GL_LEQUAL);

		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glColor4fv(CurrentLayerColor.vect);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, Main.W, Main.H);
		
		scX=pscX;
		scY=pscY;
		scW=pscW;
		scH=pscH;

		glPopAttrib();
		trX-=X;
		trY-=Y;
	}
}