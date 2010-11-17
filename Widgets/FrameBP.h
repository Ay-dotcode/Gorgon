#pragma once

#include "../Engine/GGEMain.h"
#include "../Utils/Margins.h"
#include "IWidgetBluePrint.h"
#include "../Resource/ResourceFile.h"

#define GID_FRAME		0x05880000
#define GID_FRAME_PROPS	0x05880101

namespace gorgonwidgets {
	class SliderBP;
	ResourceBase *LoadFrame(ResourceFile*,FILE*,int);

	class FrameBP : public ResourceBase, public IWidgetBluePrint {
		friend ResourceBase *LoadFrame(ResourceFile*,FILE*,int);
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
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }

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
		Guid *guid_normal;
		Guid *guid_active;
		Guid *guid_innernormal;
		Guid *guid_inneractive;
		Guid *guid_scrollingnormal;
		Guid *guid_scrollingactive;

		Guid *guid_scroller;
		ResourceFile *file;

	};

}