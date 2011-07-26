#pragma once

#include "../Resource/ResourceBase.h"
#include "../Engine/Font.h"
#include "../Resource/SoundResource.h"
#include "../Resource/ResourceFile.h"
#include "../Engine/GGEMain.h"
#include "../Resource/AnimationResource.h"
#include "../Resource/ResizableObject.h"
#include "../Utils/Margins.h"
#include <string>


namespace gge { namespace widgets {

	class CheckboxBase;

#define GID_CHECKBOX_ELEMENT			0x5250000
#define GID_CHECKBOX_ELEMENT_PROPS		0x5250101
#define GID_CHECKBOX_ELEMENT_PROPS2		0x5250102

	class CheckboxElement : public resource::ResourceBase {
		friend resource::ResourceBase *LoadCheckboxElement(resource::File*,FILE*,int);
		friend class CheckboxStyleGroup;
	public:

		enum SymbolIconOrderConstants {
			NoOverride	=-1,
			SymbolFirst = 0,
			IconFirst,
			NoIcon,
			NoSymbol,
			NoSymbolNoIcon,
		};

		CheckboxElement(void);

		virtual int getGID() { return GID_CHECKBOX_ELEMENT; }
		virtual void Prepare(gge::GGEMain &main, gge::resource::File &file);
		virtual bool Save(resource::File *File, FILE *Data) { return false; }

		CheckboxElement &Draw(WidgetLayer &layer,graphics::Colorizable2DLayer &textlayer,string &caption);
		CheckboxElement &ReadyAnimation(bool Backwards);
		CheckboxElement &Reverse();

		Font Font;
		resource::SoundResource *Sound;
		int Duration;
		graphics::RGBint ForeColor;
		graphics::RGBint ShadowColor;
		Point ShadowOffset;

		int Lines;
		SymbolIconOrderConstants SymbolIconOrder;

		Alignment SymbolAlign;
		Alignment IconAlign;
		Alignment TextAlign;

		Margins BorderMargin;
		Margins BorderWidth;
		bool AutoBorderWidth;
		Margins ContentMargin;
		Margins SymbolMargin;
		Margins IconMargin;
		Margins TextMargin;

		CheckboxBase *base;


	protected:
		FontInitiator temp_font;
		utils::SGuid sound_guid;
		resource::File *file;
		resource::ImageAnimation *symbol;
		resource::ResizableObject *border;
		resource::ResourceBase *bordertemplate;
	};
} }
