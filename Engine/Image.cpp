
#include "Image.h"
#include <cmath>
#include "..\Utils\BasicMath.h"

using namespace std;
using namespace gge::utils;

namespace gge { namespace graphics {

	void TextureImage::drawto(ImageTarget2D& Target, SizeController2D &controller, int X, int Y, int W, int H) {
		bool HTile=false, VTile=false;
		int w,h;
		if(controller.HorizontalTiling==SizeController2D::Single) {
			HTile=false;
			w=getimagewidth();

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

			if(controller.HorizontalTiling==SizeController2D::Tile_Integral_Best) {
				w=(int)Round((double)(W-controller.Horizontal.Overhead)/controller.Horizontal.Increment)*controller.Horizontal.Increment+controller.Horizontal.Overhead;
			}
			else if(controller.HorizontalTiling==SizeController2D::Tile_Integral_Smaller) {
				w=(int)floor((double)(W-controller.Horizontal.Overhead)/controller.Horizontal.Increment)*controller.Horizontal.Increment+controller.Horizontal.Overhead;
			}
			else if(controller.HorizontalTiling==SizeController2D::Tile_Integral_Fill) {
				w=(int)ceil((double)(W-controller.Horizontal.Overhead)/controller.Horizontal.Increment)*controller.Horizontal.Increment+controller.Horizontal.Overhead;
			}

			if(Alignment::isCenter(controller.Align))
				X+=(W-w)/2;
			else if(Alignment::isRight(controller.Align))
				X+=(W-w);
		}




		if(controller.VerticalTiling==SizeController2D::Single) {
			VTile=false;
			h=getimagewidth();

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

			if(controller.VerticalTiling==SizeController2D::Tile_Integral_Best) {
				h=(int)Round((double)(H-controller.Vertical.Overhead)/controller.Vertical.Increment)*controller.Vertical.Increment+controller.Vertical.Overhead;
			}
			else if(controller.VerticalTiling==SizeController2D::Tile_Integral_Smaller) {
				h=(int)floor((double)(H-controller.Vertical.Overhead)/controller.Vertical.Increment)*controller.Vertical.Increment+controller.Vertical.Overhead;
			}
			else if(controller.VerticalTiling==SizeController2D::Tile_Integral_Fill) {
				h=(int)ceil((double)(H-controller.Vertical.Overhead)/controller.Vertical.Increment)*controller.Vertical.Increment+controller.Vertical.Overhead;
			}

			if(Alignment::isMiddle(controller.Align))
				Y+=(H-h)/2;
			else if(Alignment::isBottom(controller.Align))
				Y+=(H-h);
		}
	}


	int TextureImage::calculatewidthusing(const SizeController2D &controller, int W ) const {
		if(controller.HorizontalTiling==SizeController2D::Single) {
			return getimagewidth();
		}
		else if(controller.HorizontalTiling==SizeController2D::Tile_Integral_Best) {
			return (int)Round((double)(W-controller.Horizontal.Overhead)/controller.Horizontal.Increment)*controller.Horizontal.Increment+controller.Horizontal.Overhead;
		}
		else if(controller.HorizontalTiling==SizeController2D::Tile_Integral_Smaller) {
			return (int)floor((double)(W-controller.Horizontal.Overhead)/controller.Horizontal.Increment)*controller.Horizontal.Increment+controller.Horizontal.Overhead;
		}
		else if(controller.HorizontalTiling==SizeController2D::Tile_Integral_Fill) {
			return (int)ceil((double)(W-controller.Horizontal.Overhead)/controller.Horizontal.Increment)*controller.Horizontal.Increment+controller.Horizontal.Overhead;
		}
		else
			return W;
	}

	int TextureImage::calculateheightusing(const SizeController2D &controller, int H ) const {
		if(controller.VerticalTiling==SizeController2D::Single) {
			return getimagewidth();
		}
		else if(controller.VerticalTiling==SizeController2D::Tile_Integral_Best) {
			return (int)Round((double)(H-controller.Vertical.Overhead)/controller.Vertical.Increment)*controller.Vertical.Increment+controller.Vertical.Overhead;
		}
		else if(controller.VerticalTiling==SizeController2D::Tile_Integral_Smaller) {
			return (int)floor((double)(H-controller.Vertical.Overhead)/controller.Vertical.Increment)*controller.Vertical.Increment+controller.Vertical.Overhead;
		}
		else if(controller.VerticalTiling==SizeController2D::Tile_Integral_Fill) {
			return (int)ceil((double)(H-controller.Vertical.Overhead)/controller.Vertical.Increment)*controller.Vertical.Increment+controller.Vertical.Overhead;
		}
		else
			return H;
	}



} }

