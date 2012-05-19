#include "CustomLayer.h"
#include "Graphics.h"
#include "GGEMain.h"
#include <GL/gl.h>

namespace gge {
	namespace graphics {
		extern RGBfloat CurrentLayerColor;
		extern utils::Point translate;
		extern utils::Bounds scissors;
	}

	void CustomLayer::Render() {
		using namespace gge::utils;
		using namespace gge::graphics;

		Rectangle psc;
		translate+=BoundingBox.TopLeft();
		glPushAttrib(GL_SCISSOR_BIT);
		glEnable(GL_SCISSOR_TEST);

		psc=scissors;

		int r=scissors.Right;
		int b=scissors.Bottom;

		glEnable(GL_SCISSOR_TEST);
		if(translate.x>scissors.Left)
			scissors.Left=translate.x;
		if(translate.y>scissors.Top)
			scissors.Top=translate.y;
		if(translate.y+BoundingBox.Height()<b)
			b=(translate.y+BoundingBox.Height());
		if(translate.x+BoundingBox.Width()<r)
			r=(translate.x+BoundingBox.Width());

		scissors.Right=r;
		scissors.Bottom=b;

		if(scissors.Right<=scissors.Left || scissors.Bottom<=scissors.Top) {
			return;
		}

		glScissor(scissors.Left, (ScreenSize.Height-scissors.Top)-scissors.Height(), scissors.Width(), scissors.Height());


		glColor4f(1,1,1,1);
		glViewport(BoundingBox.Left, (Main.getHeight()-BoundingBox.Top)-BoundingBox.Height(), BoundingBox.Width(), BoundingBox.Height());
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

		Renderer();

		glDepthFunc(GL_LEQUAL);

		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glColor4fv(CurrentLayerColor.vect);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, Main.getWidth(), Main.getHeight());
		
		scissors=psc;

		glPopAttrib();
		translate-=BoundingBox.TopLeft();
	}

	void CustomLayer::init() {
		Xs=-10;
		Xe= 10;
		Ys=-10;
		Ye= 10;
		Zs=-10;
		Ze= 10;
	}

}