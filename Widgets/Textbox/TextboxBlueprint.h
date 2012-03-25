#pragma once


#pragma warning(push)
#pragma warning(disable:4351)


#include "..\Base\BluePrint.h"
#include "..\Definitions.h"
#include "..\..\Resource\Sound.h"

#include <map>
#include "..\Slider\SliderBlueprint.h"
#include "..\..\Resource\Base.h"
#include "..\Checkbox\CheckboxBlueprint.h"



namespace gge { namespace widgets {

	namespace textbox {

		class Blueprint : public widgets::Blueprint, public resource::Base {
			friend Blueprint *Load(resource::File& File, std::istream &Data, int Size);
		public:


			class Element : public resource::Base {
				friend Blueprint::Element *LoadElement(resource::File& File, std::istream &Data, int Size);
				friend class Blueprint;
			public:
				Element() : InnerBorder(NULL), OuterBorder(NULL), 
					Sound(NULL), Overlay(NULL), Duration(-1), Font(NULL), SelectedFont(NULL)
				{ }

				GID::Type GetGID() const { return GID::Textbox_Element; }

				int Duration;

				BorderDataResource			*OuterBorder;
				BorderDataResource			*InnerBorder;
				gge::Font					*Font;
				gge::Font					*SelectedFont;
				resource::Sound		*Sound;
				BorderDataResource			*Overlay;


				template<class T_, int id>
				T_ *Get() const {
					throw std::runtime_error("No such variable");
				}
				template<>
				BorderDataResource *Get<BorderDataResource, 1>() const {
					return OuterBorder;
				}
				template<>
				BorderDataResource *Get<BorderDataResource, 2>() const {
					return InnerBorder;
				}
				template<>
				gge::Font *Get<gge::Font, 3>() const {
					return Font;
				}
				template<>
				gge::Font *Get<gge::Font, 4>() const {
					return SelectedFont;
				}
				template<>
				resource::Sound *Get<resource::Sound, 7>() const {
					return Sound;
				}
				template<>
				BorderDataResource *Get<BorderDataResource, 8>() const {
					return Overlay;
				}


				virtual void Prepare(GGEMain &main, resource::File &file);


				~Element() {
				}

			protected:
				utils::SGuid outerborder;
				utils::SGuid innerborder;
				FontInitiator font;
				FontInitiator selectedfont;
				utils::SGuid sound;
				utils::SGuid overlay;
			};

			Blueprint() : 	Normal(NULL), Hover(NULL), Focused(NULL), Disabled(NULL),
				NormalToHover(NULL), NormalToFocused(NULL), NormalToDisabled(NULL), HoverToFocused(NULL),
				HoverToNormal(NULL), FocusedToNormal(NULL), DisabledToNormal(NULL), FocusedToHover(NULL),
				Selection(NULL), Caret(NULL), Scroller(NULL), Align(Alignment::Middle_Left), Mapping()
			{
				DefaultSize=utils::Size(180,40);
			}


			AnimationInfo HasStyleAnimation(StyleMode style) const  {
				if(Mapping[style.from][style.to])
					return AnimationInfo(Forward, Mapping[style.from][style.to]->Duration);
				else if(Mapping[style.to][style.from])
					return AnimationInfo(Forward, Mapping[style.to][style.from]->Duration);

				return Missing;
			}


			virtual GID::Type GetGID() const { return GID::Textbox; }


			virtual void Prepare(GGEMain &main, resource::File &file);

			BorderDataResource  *Selection;
			animation::RectangularGraphic2DSequenceProvider *Caret;
			slider::Blueprint	*Scroller;
			Placeholder			CaretPlace;
			Alignment::Type		Align;
			resource::Sound *TypeSound;


			Element *Normal;
			Element *Hover;
			Element *Focused;
			Element *Disabled;

			Element *NormalToHover;
			Element *NormalToFocused;
			Element *NormalToDisabled;
			Element *HoverToFocused;

			Element *HoverToNormal;
			Element *FocusedToNormal;
			Element *DisabledToNormal;
			Element *FocusedToHover;

			template<class T_, int id>
			T_ *Get(StyleMode style, bool &transition) const {
				if(Mapping[style.from][style.to] && Mapping[style.from][style.to]->Get<T_,id>()) {
					if(style.to!=0)
						transition=true;
					else
						transition=false;

					return Mapping[style.from][style.to]->Get<T_,id>();
				}

				if(style.to!=Blueprint::Style_None) {
					if(Mapping[style.to][style.from] && Mapping[style.to][style.from]->Get<T_,id>()) {
						transition=true;

						return Mapping[style.to][style.from]->Get<T_,id>();
					}
				}

				if(style.to!=Blueprint::Style_None) {
					if(Mapping[style.from][Blueprint::Style_None] && Mapping[style.from][Blueprint::Style_None]->Get<T_,id>()) {
						transition=false;

						return Mapping[style.from][Blueprint::Style_None]->Get<T_,id>();
					}
				}

				if(style.from!=widgets::Blueprint::Normal && style.to!=widgets::Blueprint::Normal) {
					if(Mapping[widgets::Blueprint::Normal][Blueprint::Style_None] && Mapping[widgets::Blueprint::Normal][Blueprint::Style_None]->Get<T_,id>()) {
						transition=false;

						return Mapping[widgets::Blueprint::Normal][Blueprint::Style_None]->Get<T_,id>();
					}
				}

				return NULL;
			}
			BorderDataResource *GetOuterBorder(StyleMode style, bool &transition) const {
				return Get<BorderDataResource, 1>(style, transition);
			}
			BorderDataResource *GetInnerBorder(StyleMode style, bool &transition) const {
				return Get<BorderDataResource, 2>(style, transition);
			}
			Font *GetFont(StyleMode style, bool &transition) const {
				return Get<Font, 3>(style, transition);
			}
			Font *GetSelectedFont(StyleMode style, bool &transition) const {
				return Get<Font, 4>(style, transition);
			}
			resource::Sound *GetSound(StyleMode style, bool &type) const {
				return Get<resource::Sound, 7>(style, type);
			}
			BorderDataResource *GetOverlay(StyleMode style, bool &transition) const {
				return Get<BorderDataResource, 8>(style, transition);
			}

			Element *Mapping[6][6];


		protected:
			utils::SGuid selection;
			utils::SGuid caret;
			utils::SGuid caretplace;
			utils::SGuid scroller;
			utils::SGuid typesound;

			utils::SGuid normal;
			utils::SGuid hover;
			utils::SGuid focused;
			utils::SGuid disabled;

			utils::SGuid normaltohover;
			utils::SGuid normaltofocused;
			utils::SGuid normaltodisabled;
			utils::SGuid hovertofocused;

			utils::SGuid hovertonormal;
			utils::SGuid focusedtonormal;
			utils::SGuid disabledtonormal;
			utils::SGuid focusedtohover;


		};

	}
} }


#pragma warning(pop)
