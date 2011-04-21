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
	class ResourceFile;

	ResourceBase *LoadFontTheme(ResourceFile* File, FILE* Data, int Size);

	class FontTheme : public ResourceBase {
		friend ResourceBase *LoadFontTheme(ResourceFile* File, FILE* Data, int Size);

	public:

		virtual int getGID() { return GID_FONTTHEME; }

		////Currently does nothing
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }

		FontTheme() : ResourceBase(),
			NormalFont(NULL),
			BoldFont(NULL),
			ItalicFont(NULL),
			SmallFont(NULL),
			H1Font(NULL),
			H2Font(NULL),
			H3Font(NULL),
			guid_normal(NULL),
			guid_bold(NULL),
			guid_italic(NULL),
			guid_small(NULL),
			guid_h1(NULL),
			guid_h2(NULL),
			guid_h3(NULL)
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
			guid_normal(NULL),
			guid_bold(NULL),
			guid_italic(NULL),
			guid_small(NULL),
			guid_h1(NULL),
			guid_h2(NULL),
			guid_h3(NULL)
		{ }

		virtual void Prepare(GGEMain *main);

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

		Guid			*guid_normal,
						*guid_bold,
						*guid_italic,
						*guid_small,
						*guid_h1,
						*guid_h2,
						*guid_h3
		;

		ResourceFile *file;

	};
} }
