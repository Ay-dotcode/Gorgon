#pragma once

#include "../Engine/GGEMain.h"
#include "../Utils/Margins.h"
#include "IWidgetBluePrint.h"
#include "../Resource/ResourceFile.h"

#define GID_FRAME		0x05880000
#define GID_FRAME_PROPS	0x05880101

namespace gge { namespace widgets {
	class SliderBP;
	resource::ResourceBase *LoadFrame(resource::File*,FILE*,int);

	class FrameBP : public resource::ResourceBase, public IWidgetBluePrint {
		friend resource::ResourceBase *LoadFrame(resource::File*,FILE*,int);
	public:

		FrameBP(RectangleResource *normal=NULL, SliderBP *scroller=NULL, Pointer::PointerTypes pointer=Pointer::None) 
		  : 
			Normal(normal), Active(NULL),
			InnerNormal(NULL), InnerActive(NULL),
			ScrollingNormal(NULL), ScrollingActive(NULL),
			Scroller(scroller), 
			OuterBorderWidth(5), ScrollbarMargin(2),
			InnerBorderWidth(5), InnerBorderMargin(5), 
			ScrollingBorderWidth(5), ScrollingBorderMargin(5), 
			ContentMargin(5),
			AutoBorderWidth(true)
		{ PointerType=pointer; }
		
		virtual IWidgetObject *Create(IWidgetContainer& Container,int X,int Y,int Width,int Height) {
			return NULL;//new Frame(*this, X, Y, Width, Height);
		}

		virtual int getGID() { return GID_FRAME; }
		virtual bool Save(resource::File *File, FILE *Data) { return false; }

		RectangleResource *Normal;
		RectangleResource *Active;

		RectangleResource *InnerNormal;
		RectangleResource *InnerActive;

		RectangleResource *ScrollingNormal;
		RectangleResource *ScrollingActive;

		SliderBP* Scroller;

		Margins OuterBorderWidth;
		Margins ScrollbarMargin;
		Margins InnerBorderMargin;
		Margins InnerBorderWidth;
		Margins ScrollingBorderMargin;
		Margins ScrollingBorderWidth;
		Margins ContentMargin;
		
		bool AutoBorderWidth;

		virtual void Prepare(GGEMain *main);

	protected:
		utils::SGuid guid_normal;
		utils::SGuid guid_active;
		utils::SGuid guid_innernormal;
		utils::SGuid guid_inneractive;
		utils::SGuid guid_scrollingnormal;
		utils::SGuid guid_scrollingactive;

		utils::SGuid guid_scroller;
		resource::File *file;

	};

} }
