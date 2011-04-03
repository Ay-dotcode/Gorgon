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

using namespace gre;
using namespace gge;
using namespace std;

namespace gge { namespace widgets {

	class CheckboxBase;

#define GID_CHECKBOX_ELEMENT			0x5250000
#define GID_CHECKBOX_ELEMENT_PROPS		0x5250101
#define GID_CHECKBOX_ELEMENT_PROPS2		0x5250102

	class CheckboxElement : public ResourceBase {
		friend ResourceBase *LoadCheckboxElement(ResourceFile*,FILE*,int);
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
		virtual void Prepare(gge::GGEMain *main);
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }

		CheckboxElement &Draw(WidgetLayer &layer,Colorizable2DLayer &textlayer,string &caption);
		CheckboxElement &ReadyAnimation(bool Backwards);
		CheckboxElement &Reverse();

		Font Font;
		SoundResource *Sound;
		int Duration;
		RGBint ForeColor;
		RGBint ShadowColor;
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
		Guid *sound_guid;
		ResourceFile *file;
		ImageAnimation *symbol;
		ResizableObject *border;
		ResourceBase *bordertemplate;
	};
} }
