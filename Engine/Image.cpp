
#include "Image.h"
#include <cmath>
#include "..\Utils\BasicMath.h"

using namespace std;
using namespace gge::utils;

namespace gge { namespace graphics {

	void ImageTexture::drawin(ImageTarget2D& Target, const SizeController2D &controller, int X, int Y, int W, int H) {
		bool HTile=false, VTile=false;
		int w=W,h=H;
		if(controller.HorizontalTiling==SizeController2D::Single) {
			HTile=false;
			w=getwidth();

			if(Alignment::isCenter(controller.Align))
				X+=(W-w)/2;
			else if(Alignment::isRight(controller.Align))
				X+=(W-w);

		}
		else if(controller.HorizontalTiling==SizeController2D::Tile_Continous) {
			HTile=true;

		}
		else if(controller.HorizontalTiling==SizeController2D::Stretch) {
			HTile=false;
		}
		else {
			HTile=true;

			w=calculatewidth(controller, W);

			if(Alignment::isCenter(controller.Align))
				X+=(W-w)/2;
			else if(Alignment::isRight(controller.Align))
				X+=(W-w);
		}




		if(controller.VerticalTiling==SizeController2D::Single) {
			VTile=false;
			h=getheight();

			if(Alignment::isMiddle(controller.Align))
				Y+=(H-h)/2;
			else if(Alignment::isBottom(controller.Align))
				Y+=(H-h);

		}
		else if(controller.VerticalTiling==SizeController2D::Tile_Continous) {
			VTile=true;

		}
		else if(controller.VerticalTiling==SizeController2D::Stretch) {
			VTile=false;
		}
		else {
			VTile=true;

			h=calculateheight(controller, H);

			if(Alignment::isMiddle(controller.Align))
				Y+=(H-h)/2;
			else if(Alignment::isBottom(controller.Align))
				Y+=(H-h);
		}

		Target.Draw(&Texture, Tiling2D::Tile(HTile, VTile), X,Y,w,h);
	}




} }

