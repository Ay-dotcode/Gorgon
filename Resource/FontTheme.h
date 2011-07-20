#pragma once


#include "GRE.h"
#include "ResourceBase.h"
#include "ImageResource.h"
#include "../Engine/FontRenderer.h"
#include "../Engine/GGEMain.h"
#include "../Engine/Font.h"

#define		GID_FONTTHEME		0x03300000
#define		GID_FONTTHEME_PROPS	0x03300804

namespace gge { namespace resource {
	class File;

	ResourceBase *LoadFontTheme(File &File, std::istream &Data, int Size);

	class FontTheme : public ResourceBase {
		friend ResourceBase *LoadFontTheme(File &File, std::istream &Data, int Size);

	public:

		virtual GID::Type getGID() const { return GID::FontTheme; }

		////Currently does nothing
		virtual bool Save(File &File, std::ostream &Data) { return false; }

		FontTheme() : ResourceBase(),
			NormalFont(nullptr),
			BoldFont(nullptr),
			ItalicFont(nullptr),
			SmallFont(nullptr),
			H1Font(nullptr),
			H2Font(nullptr),
			H3Font(nullptr),
			guid_normal(nullptr),
			guid_bold(nullptr),
			guid_italic(nullptr),
			guid_small(nullptr),
			guid_h1(nullptr),
			guid_h2(nullptr),
			guid_h3(nullptr)
		{ }

		FontTheme(FontRenderer *Normal,FontRenderer *Bold=NULL,FontRenderer *Italic=NULL,
				  FontRenderer *Small=NULL,
				  FontRenderer *H1=NULL,FontRenderer *H2=NULL,FontRenderer *H3=NULL) : 
		    ResourceBase(),
			NormalFont(Normal),
			BoldFont(Bold),
			ItalicFont(Italic),
			SmallFont(Small),
			H1Font(H1),
			H2Font(H2),
			H3Font(H3),
			guid_normal(nullptr),
			guid_bold(nullptr),
			guid_italic(nullptr),
			guid_small(nullptr),
			guid_h1(nullptr),
			guid_h2(nullptr),
			guid_h3(nullptr)
		{ }

		virtual void Prepare(GGEMain &main);

		virtual FontRenderer *DetermineRenderer(Font::FontStyle Style=Font::Normal) {
			if(Style==Font::Normal)
				return NormalFont;
			
			if(Style==Font::Bold) {
				if(BoldFont!=NULL)
					return BoldFont;
				else
					return NormalFont;
			}

			if(Style==Font::Italic) {
				if(ItalicFont!=NULL)
					return ItalicFont;
				else
					return NormalFont;
			}

			if(Style==Font::Small) {
				if(SmallFont!=NULL)
					return SmallFont;
				else
					return NormalFont;
			}

			if(Style==Font::H1) {
				if(H1Font!=NULL)
					return H1Font;
				else if(H2Font!=NULL)
					return H2Font;
				else
					return DetermineRenderer(Font::Bold);
			}

			if(Style==Font::H2) { 
				if(H2Font!=NULL)
					return H2Font;
				else 
					return DetermineRenderer(Font::H1);
			}

			if(Style==Font::H3) { 
				if(H3Font!=NULL)
					return H3Font;
				else
					return DetermineRenderer(Font::Bold);
			}

			return NULL;
		}

		void setRenderer(Font::FontStyle style, FontRenderer *renderer) {
			switch(style) {
				case Font::Normal:
					NormalFont=renderer;
					break;
				case Font::Bold:
					BoldFont=renderer;
					break;
				case Font::Italic:
					ItalicFont=renderer;
					break;
				case Font::Small:
					SmallFont=renderer;
					break;
				case Font::H1:
					H1Font=renderer;
					break;
				case Font::H2:
					H2Font=renderer;
					break;
				case Font::H3:
					H3Font=renderer;
					break;
			}
		}

		void setRenderer(Font::FontStyle style, FontRenderer &renderer) {
			setRenderer(style, &renderer);
		}


	protected:
		FontRenderer	*NormalFont,
						*BoldFont,
						*ItalicFont,
						*SmallFont,
						*H1Font,
						*H2Font,
						*H3Font
		;

		utils::SGuid	guid_normal,
						guid_bold,
						guid_italic,
						guid_small,
						guid_h1,
						guid_h2,
						guid_h3
		;

		File *file;

	};
} }
