#pragma once

#include "../Resource/ResourceBase.h"
#include "../Resource/BitmapFontResource.h"
#include "../Resource/SoundResource.h"
#include "../Resource/ResourceFile.h"
#include "../Engine/GGEMain.h"
#include "../Resource/AnimationResource.h"
#include "../Resource/ResizableObject.h"
#include "Line.h"
#include "Rectangle.h"
#include <string>

#define GID_SLIDER_ELEMENT			0x05260000
#define GID_SLIDER_ELEMENT_PROPS	0x05260101


namespace gge { namespace widgets {
	class Slider;

	enum SliderStyles {
		SS_Top,
		SS_Left,
		SS_Right,
		SS_Bottom,
		SS_Horizontal,
		SS_Verticle
	};

	struct SliderLocationName {
		string name;
		float value;
		void *data;

		SliderLocationName(string &Name, void *data) {
			name=Name;
			this->data=data;
		}

		SliderLocationName(string &Name, float Value, void *data) {
			name=Name;
			value=Value;
			this->data=data;
		}
	};

	class SliderElement : public resource::ResourceBase
	{
		friend resource::ResourceBase *LoadSliderElement(resource::File*,FILE*,int);
		friend class SliderStyleGroup;
		friend class SliderBase;
	public:
		SliderElement(void);

		virtual int getGID() { return GID_SLIDER_ELEMENT; }
		virtual bool Save(resource::File *File, FILE *Data) { return false; }
		virtual void Prepare(gge::GGEMain &main, gge::resource::File &file);

		SliderElement &DrawRule(WidgetLayer &Target);
		SliderElement &DrawTickMarks(WidgetLayer &Target, float Distance);
		SliderElement &DrawTickNumbers(graphics::Colorizable2DLayer &Target, float Distance, float Start, float Increment, float End, string Format);
		SliderElement &DrawTickNames(graphics::Colorizable2DLayer &Target, float Distance, utils::Collection<SliderLocationName> &Texts,bool useLocations, float min, float max);
		SliderElement &DrawLocatedTicks(WidgetLayer &Target, float Distance, utils::Collection<SliderLocationName> &Texts, float min, float max);
		SliderElement &DrawSymbol(WidgetLayer &Target);
		SliderElement &DrawOverlay(graphics::Basic2DLayer &Target);
		SliderElement &ReadyAnimation(bool Backwards);
		SliderElement &Reverse();

		gge::Rectangle RuleRectangle(WidgetLayer &Target, Point up=Point(0,0), Point down=Point(0,0), Point dist=Point(0,0));

		int Duration;

		resource::BitmapFontResource *Font;
		resource::SoundResource *Sound;
		graphics::RGBint ForeColor,ShadowColor;
		Point Offset;
		Point ShadowOffset;
		Point TextOffset;
		Point SymbolOffset;
		Point TickOffset;
		Point OverlayOffset;
		TextAlignment TextAlign;
		int RuleOffset;
		int StartMargin;
		int EndMargin;
		int MinTickDistance;
		int OverlaySizeMod;
		
		void setStyle(SliderStyles style);


	protected:
		utils::SGuid font_guid;
		utils::SGuid sound_guid;
		resource::File *file;

		SliderStyles style;

		resource::ResizableObject *Rule;
		resource::ResizableObject *Overlay;
		resource::ResizableObject *Symbol;
		graphics::Buffered2DGraphic *Tick;
	};
} }
