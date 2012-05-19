#include "Rectangle.h"
#include "../../Resource/File.h"
#include "../../Engine/Animation.h"
#include "../../Resource/Animation.h"
#include "../../Resource/NullImage.h"
#include <vector>
#include "../../Utils/SGuid.h"

using namespace gge::utils;
using namespace gge::resource;
using namespace gge::animation;
using namespace gge::graphics;
using namespace std;

namespace gge { namespace widgets {


	namespace rectangleresource_prvt {
		void setrectangleprovider(vector<Animation *> &anims, RectangularGraphic2DSequenceProvider *&obj, int id) {
			if((int)anims.size()<=id) {
				obj=&NullImage::Get();
			}
			else {
				switch(anims[id]->GetNumberofFrames()) {
				case 0:
					obj=&NullImage::Get();
					break;
				case 1:
					obj=&((*(anims[id]))[0]);
					break;
				default:
					obj=anims[id];
				}
			}
		}
	}

	RectangleResource *LoadRectangleResource(resource::File& File, std::istream &Data, int Size) {
		RectangleResource *rectangle;
		SGuid g;
		vector<Animation *> anims;
		RectangularGraphic2DSequenceProvider *tl=NULL, *t=NULL, *tr=NULL;
		RectangularGraphic2DSequenceProvider * l=NULL, *c=NULL, * r=NULL;
		RectangularGraphic2DSequenceProvider *bl=NULL, *b=NULL, *br=NULL;
		SGuid mask;
		RectangleResource::TilingInfo tile;
		tile.Top=true;
		tile.Left=true;
		tile.Bottom=true;
		tile.Right=true;
		tile.Center_Horizontal=true;
		tile.Center_Vertical=true;


		int target=Data.tellg()+Size;
		while(Data.tellg()<target) {
			int gid,size;
			ReadFrom(Data, gid);
			ReadFrom(Data, size);

			if(gid==resource::GID::Guid) {
				g.LoadLong(Data);
			}
			else if(gid==resource::GID::SGuid) {
				g.Load(Data);
			}
			else if(gid==GID::Rectangle_Props) {
				mask.Load(Data);
				tile.Center_Horizontal=ReadFrom<int>(Data)!=0;
				tile.Center_Vertical=ReadFrom<int>(Data)!=0;
				tile.Top=ReadFrom<int>(Data)!=0;
				tile.Bottom=ReadFrom<int>(Data)!=0;
				tile.Left=ReadFrom<int>(Data)!=0;
				tile.Right=ReadFrom<int>(Data)!=0;

				if(size>32)
					EatChunk(Data,size-32);
			}
			else if(gid==resource::GID::Animation) {
				anims.push_back(LoadAnimationResource(File, Data, size));
			}
			else {
				EatChunk(Data, size);
			}
		}

		rectangleresource_prvt::setrectangleprovider(anims, t , 0);
		rectangleresource_prvt::setrectangleprovider(anims, tl, 1);
		rectangleresource_prvt::setrectangleprovider(anims, tr, 2);

		rectangleresource_prvt::setrectangleprovider(anims, c, 3);
		rectangleresource_prvt::setrectangleprovider(anims, l, 4);
		rectangleresource_prvt::setrectangleprovider(anims, r, 5);

		rectangleresource_prvt::setrectangleprovider(anims, b , 6);
		rectangleresource_prvt::setrectangleprovider(anims, bl, 7);
		rectangleresource_prvt::setrectangleprovider(anims, br, 8);

		rectangle=new RectangleResource(*tl, *t, *tr, *l, *c, *r, *bl, *b, *br, tile);
		rectangle->guid=g;

		for(auto i=anims.begin(); i!=anims.end(); i++) {
			rectangle->Subitems.Add(*i, rectangle->Subitems.HighestOrder()+1);
		}

		rectangle->mask=mask;
		return rectangle;
	}


	void Rectangle::drawin(graphics::ImageTarget2D& Target, int X, int Y, int W, int H) const {

		RectangleResource::TilingInfo tiling=parent.Tiling;

		//TOP
		tl->Draw(Target, X, Y);

		t->Draw(Target, Tiling2D::Tile(tiling.Top, false), 
			X+tl->GetWidth(), Y,
			W - (tl->GetWidth() + tr->GetWidth()), tr->GetHeight()
		);

		tr->Draw(Target, X+(W-tr->GetWidth()), Y);



		//CENTER
		l->Draw(Target, Tiling2D::Tile(false, tiling.Left), 
			X, Y+tl->GetHeight(),
			l->GetWidth(), H-(tl->GetHeight()+bl->GetHeight())
		);

		c->Draw(Target, Tiling2D::Tile(tiling.Center_Horizontal, tiling.Center_Vertical), 
			X+l->GetWidth(), Y+t->GetHeight(),
			W-(l->GetWidth()+r->GetWidth()), H-(t->GetHeight()+b->GetHeight())
		);

		r->Draw(Target, Tiling2D::Tile(false, tiling.Right), 
			X+(H-r->GetWidth()), Y+tr->GetHeight(),
			r->GetWidth(), H-(tr->GetHeight()+br->GetHeight())
		);


		
		//BOTTOM
		bl->Draw(Target, X, Y+(H-bl->GetHeight()));

		b->Draw(Target, Tiling2D::Tile(tiling.Bottom, false), 
			X+tl->GetWidth(), Y+(H-b->GetHeight()),
			W - (bl->GetWidth() + br->GetWidth()), br->GetHeight()
		);

		br->Draw(Target, X+(W-br->GetWidth()), Y+(H-br->GetHeight()));
	}

	void Rectangle::drawin(graphics::ImageTarget2D& Target, const graphics::SizeController2D &controller, int X, int Y, int W, int H) const {

		RectangleResource::TilingInfo tiling=parent.Tiling;
		int w=W, h=H;

		W=controller.CalculateWidth(W, c->GetWidth(), l->GetWidth()+r->GetWidth());
		H=controller.CalculateHeight(H, c->GetWidth(), t->GetHeight()+b->GetHeight());

		Point p=Alignment::CalculateLocation(controller.Align, utils::Rectangle(X,Y,W,H),Size(w,h));
		X=p.x;
		Y=p.y;

		//TOP
		tl->Draw(Target, X, Y);

		t->Draw(Target, Tiling2D::Tile(tiling.Top, false), 
			X+tl->GetWidth(), Y,
			W - (tl->GetWidth() + tr->GetWidth()), tr->GetHeight()
		);

		tr->Draw(Target, X+(W-tr->GetWidth()), Y);



		//CENTER
		l->Draw(Target, Tiling2D::Tile(false, tiling.Left), 
			X, Y+tl->GetHeight(),
			l->GetWidth(), H-(tl->GetHeight()+bl->GetHeight())
		);

		c->Draw(Target, Tiling2D::Tile(tiling.Center_Horizontal, tiling.Center_Vertical), 
			X+l->GetWidth(), Y+t->GetHeight(),
			W-(l->GetWidth()+r->GetWidth()), H-(t->GetHeight()+b->GetHeight())
		);

		r->Draw(Target, Tiling2D::Tile(false, tiling.Right), 
			X+(W-r->GetWidth()), Y+tr->GetHeight(),
			r->GetWidth(), H-(tr->GetHeight()+br->GetHeight())
		);



		//BOTTOM
		bl->Draw(Target, X, Y+(H-bl->GetHeight()));

		b->Draw(Target, Tiling2D::Tile(tiling.Bottom, false), 
			X+tl->GetWidth(), Y+(H-b->GetHeight()),
			W - (bl->GetWidth() + br->GetWidth()), br->GetHeight()
		);

		br->Draw(Target, X+(W-br->GetWidth()), Y+(H-br->GetHeight()));
	}

	void MaskedRectangle::drawin(graphics::ImageTarget2D& Target, int X, int Y, int W, int H) const {

		RectangleResource::TilingInfo tiling=parent.Tiling;

		Target.SetDrawMode(graphics::BasicSurface::Offscreen);

		//CENTER
		c->Draw(Target, Tiling2D::Tile(tiling.Center_Horizontal, tiling.Center_Vertical), 
			X+l->GetWidth(), Y+t->GetHeight(),
			W-(l->GetWidth()+r->GetWidth()), H-(t->GetHeight()+b->GetHeight())
		);

		Target.SetDrawMode(graphics::BasicSurface::OffscreenAlphaOnly);
		Mask->DrawIn(Target, X,Y, W,H);

		Target.SetDrawMode(graphics::BasicSurface::Normal);



		//TOP
		tl->Draw(Target, X, Y);

		t->Draw(Target, Tiling2D::Tile(tiling.Top, false), 
			X+tl->GetWidth(), Y,
			W - (tl->GetWidth() + tr->GetWidth()), tr->GetHeight()
		);

		tr->Draw(Target, X+(W-tr->GetWidth()), Y);



		//CENTER
		l->Draw(Target, Tiling2D::Tile(false, tiling.Left), 
			X, Y+tl->GetHeight(),
			l->GetWidth(), H-(tl->GetHeight()+bl->GetHeight())
		);

		r->Draw(Target, Tiling2D::Tile(false, tiling.Right), 
			X+(H-r->GetWidth()), Y+tr->GetHeight(),
			r->GetWidth(), H-(tr->GetHeight()+br->GetHeight())
		);



		//BOTTOM
		bl->Draw(Target, X, Y+(H-bl->GetHeight()));

		b->Draw(Target, Tiling2D::Tile(tiling.Bottom, false), 
			X+tl->GetWidth(), Y+(H-b->GetHeight()),
			W - (bl->GetWidth() + br->GetWidth()), br->GetHeight()
		);

		br->Draw(Target, X+(W-br->GetWidth()), Y+(H-br->GetHeight()));
	}

	void MaskedRectangle::drawin(graphics::ImageTarget2D& Target, const graphics::SizeController2D &controller, int X, int Y, int W, int H) const {
		Target.SetDrawMode(graphics::BasicSurface::Offscreen);

		RectangleResource::TilingInfo tiling=parent.Tiling;
		int w=W, h=H;

		W=controller.CalculateWidth(W, c->GetWidth(), l->GetWidth()+r->GetWidth());
		H=controller.CalculateHeight(H, c->GetWidth(), t->GetHeight()+b->GetHeight());

		Point p=Alignment::CalculateLocation(controller.Align, utils::Rectangle(X,Y,W,H),Size(w,h));
		X=p.x;
		Y=p.y;

		//CENTER
		c->Draw(Target, Tiling2D::Tile(tiling.Center_Horizontal, tiling.Center_Vertical), 
			X, Y,
			W, H
		);

		Target.SetDrawMode(graphics::BasicSurface::OffscreenAlphaOnly);
		Mask->DrawIn(Target, controller, X,Y, W,H);

		Target.SetDrawMode(graphics::BasicSurface::Normal);
		//return;

		//TOP
		tl->Draw(Target, X, Y);

		t->Draw(Target, Tiling2D::Tile(tiling.Top, false), 
			X+tl->GetWidth(), Y,
			W - (tl->GetWidth() + tr->GetWidth()), tr->GetHeight()
		);

		tr->Draw(Target, X+(W-tr->GetWidth()), Y);



		//CENTER
		l->Draw(Target, Tiling2D::Tile(false, tiling.Left), 
			X, Y+tl->GetHeight(),
			l->GetWidth(), H-(tl->GetHeight()+bl->GetHeight())
		);

		r->Draw(Target, Tiling2D::Tile(false, tiling.Right), 
			X+(W-r->GetWidth()), Y+tr->GetHeight(),
			r->GetWidth(), H-(tr->GetHeight()+br->GetHeight())
		);



		//BOTTOM
		bl->Draw(Target, X, Y+(H-bl->GetHeight()));

		b->Draw(Target, Tiling2D::Tile(tiling.Bottom, false), 
			X+tl->GetWidth(), Y+(H-b->GetHeight()),
			W - (bl->GetWidth() + br->GetWidth()), br->GetHeight()
		);

		br->Draw(Target, X+(W-br->GetWidth()), Y+(H-br->GetHeight()));

	}

	animation::ProgressResult::Type Rectangle::Progress() {
		return animation::ProgressResult::None;
	}


}}