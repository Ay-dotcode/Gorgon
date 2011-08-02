
#include "Line.h"
#include "..\..\Resource\ResourceFile.h"
#include "..\..\Engine\Animation.h"
#include "..\..\Resource\AnimationResource.h"
#include "..\..\Resource\NullImage.h"
#include <vector>
#include "..\..\Utils\SGuid.h"

using namespace gge::utils;
using namespace gge::resource;
using namespace gge::animation;
using namespace gge::graphics;
using namespace std;

namespace gge { namespace widgets {

	namespace lineresource_prvt {
		void setlineprovider(vector<AnimationResource *> &anims, RectangularGraphic2DSequenceProvider *&obj, int id) {
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

	LineResource *LoadLineResource(resource::File& File, std::istream &Data, int Size) {
		LineResource *line;
		SGuid g;
		vector<AnimationResource *> anims;
		RectangularGraphic2DSequenceProvider *start=NULL, *loop=NULL, *end=NULL;
		bool istiled=true;
		LineResource::OrientationType orientation=LineResource::Horizontal;
		
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
			else if(gid==GID::Line_Props) {
				istiled = (ReadFrom<int>(Data) != 0);
				orientation = (ReadFrom<int>(Data)!=0 ? LineResource::Vertical : LineResource::Horizontal);

				if(size>8)
					EatChunk(Data,size-8);
			}
			else if(gid==resource::GID::Animation) {
				anims.push_back(LoadAnimationResource(File, Data, size));
			}
			else {
				EatChunk(Data, size);
			}
		}

		lineresource_prvt::setlineprovider(anims, start, 0);
		lineresource_prvt::setlineprovider(anims, loop, 1);
		lineresource_prvt::setlineprovider(anims, end, 2);

		line=new LineResource(*start, *loop, *end, orientation,istiled);
		line->guid=g;

		for(auto i=anims.begin(); i!=anims.end(); i++) {
			line->Subitems.Add(*i, line->Subitems.HighestOrder()+1);
		}

		return line;
	}


	void Line::drawin(graphics::ImageTarget2D& Target, int X, int Y, int W, int H) {
		RectangularGraphic2D &start=*this->start;
		RectangularGraphic2D &loop =*this->loop;
		RectangularGraphic2D &end  =*this->end;
		
		if(parent.Orientation==LineResource::Horizontal) {
			start.Draw(Target, X, Y+(H-start.GetHeight())/2);

			loop.Draw(Target, Tiling2D::Tile(parent.IsLoopTiled, false), 
				X+start.GetWidth(), Y+(H-loop.GetHeight())/2,
				W - (start.GetWidth() + end.GetWidth()), loop.GetHeight()
			);

			end.Draw(Target, X+W-end.GetWidth(), Y+(H-end.GetHeight())/2);
		}
		else {
			start.Draw(Target, X+(W-start.GetWidth())/2, Y);

			loop.Draw(Target, Tiling2D::Tile(false, parent.IsLoopTiled), 
				X+(W-loop.GetWidth())/2, Y+start.GetHeight(),
				loop.GetWidth(), H - (start.GetHeight() + end.GetHeight())
			);

			end.Draw(Target, X+(W-end.GetWidth())/2, Y+H-end.GetHeight());
		}
	}

	void Line::drawin(graphics::ImageTarget2D& Target, const graphics::SizeController2D &controller, int X, int Y, int W, int H) {
		RectangularGraphic2D &start=*this->start;
		RectangularGraphic2D &loop =*this->loop;
		RectangularGraphic2D &end  =*this->end;

		if(parent.Orientation==LineResource::Horizontal) {
			int w, h;

			w=controller.CalculateWidth(W, loop.GetWidth(), start.GetWidth()+end.GetWidth());
			h=controller.CalculateHeight(H, loop.GetHeight());

			Point p=Alignment::CalculateLocation(controller.Align, Rectangle(X,Y,w,h), Size(W,H));
			X=p.x;
			Y=p.y;

			start.DrawIn(Target, controller, X, Y, start.GetWidth(), h);

			graphics::SizeController2D c=controller;

			if(parent.IsLoopTiled)
				c.HorizontalTiling=SizeController2D::Tile_Continous;
			else
				c.HorizontalTiling=SizeController2D::Stretch;

			loop.DrawIn(Target, c, 
				X+start.GetWidth(), Y,
				w - (start.GetWidth() + end.GetWidth()), h
			);

			end.DrawIn(Target, controller, X+w-end.GetWidth(), Y, end.GetWidth(), h);
		}
		else {
			int w, h;

			w=controller.CalculateWidth(W, loop.GetWidth());
			h=controller.CalculateHeight(H, loop.GetHeight(), start.GetHeight()+end.GetHeight());

			Point p=Alignment::CalculateLocation(controller.Align, Rectangle(X,Y,w,h), Size(W,H));

			start.DrawIn(Target, controller, X, Y, w, start.GetHeight());

			graphics::SizeController2D c=controller;

			if(parent.IsLoopTiled)
				c.VerticalTiling=SizeController2D::Tile_Continous;
			else
				c.VerticalTiling=SizeController2D::Stretch;

			loop.DrawIn(Target, c, 
				X, Y+start.GetHeight(),
				w, h - (start.GetHeight() + end.GetHeight())
			);

			end.DrawIn(Target, controller, X, Y+H-end.GetHeight(), w, end.GetHeight());
		}
	}

	animation::ProgressResult::Type Line::Progress() {
		return animation::ProgressResult::None;
	}

}}
