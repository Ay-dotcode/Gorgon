#pragma once


#pragma warning(push)
#pragma warning(disable:4351)


#include "../Base/Blueprint.h"
#include "../Definitions.h"
#include "../../Resource/Sound.h"

#include <map>
#include "../Slider/SliderBlueprint.h"
#include "../../Resource/Base.h"
#include "../Checkbox/CheckboxBlueprint.h"



namespace gge { namespace widgets {

	namespace panel {
		
		class Blueprint;
		Blueprint *Load(resource::File& File, std::istream &Data, int Size);

		class Blueprint : public widgets::Blueprint, public resource::Base {
			friend Blueprint *Load(resource::File& File, std::istream &Data, int Size);
		public:


			class Element : public resource::Base {
				friend Blueprint::Element *LoadElement(resource::File& File, std::istream &Data, int Size);
				friend class Blueprint;
			public:
				Element() : InnerBorder(NULL), OuterBorder(NULL), ScrollingBorder(NULL),
					Resizer(NULL), ResizerPlace(NULL), TitleBorder(NULL), DialogButtonBorder(NULL),
					Sound(NULL), Overlay(NULL), Duration(-1), Opacity(255), BGOpacity(255)
				{ }

				GID::Type GetGID() const { return GID::Panel_Element; }

				int Duration;

				BorderDataResource			*OuterBorder;
				BorderDataResource			*InnerBorder;
				BorderDataResource			*ScrollingBorder;
				animation::RectangularGraphic2DSequenceProvider *Resizer;
				Placeholder					*ResizerPlace;
				Byte						 Opacity;
				Byte						 BGOpacity;
				BorderDataResource			*TitleBorder;
				BorderDataResource			*DialogButtonBorder;
				resource::Sound				*Sound;
				BorderDataResource			*Overlay;


				template<class T_, int id>
				T_ *Get() const {
					throw std::runtime_error("No such variable");
				}


				virtual void Prepare(GGEMain &main, resource::File &file);


				~Element() {
				}

			protected:
				utils::SGuid outerborder;
				utils::SGuid innerborder;
				utils::SGuid scrollingborder;
				utils::SGuid resizer;
				utils::SGuid resizerplace;
				utils::SGuid titleborder;
				utils::SGuid dialogbuttonborder;
				utils::SGuid sound;
				utils::SGuid overlay;
			};

			Blueprint() : Scroller(NULL), TitleLabel(NULL), TitleButton(NULL), TitleButtonPlace(NULL),
				DialogButton(NULL), DialogButtonPlace(NULL), 
				Normal(NULL), Active(NULL), Moving(NULL), Disabled(NULL),
				NormalToActive(NULL), NormalToDisabled(NULL), ActiveToMoving(NULL),
				ActiveToNormal(NULL), DisabledToNormal(NULL), MovingToActive(NULL)
			{
				DefaultSize=utils::Size(300,400);
			}


			AnimationInfo HasStyleAnimation(StyleMode style) const  {
				if(Mapping[style.from][style.to])
					return AnimationInfo(Forward, Mapping[style.from][style.to]->Duration);
				else if(Mapping[style.to][style.from])
					return AnimationInfo(Forward, Mapping[style.to][style.from]->Duration);

				return Missing;
			}


			virtual GID::Type GetGID() const { return GID::Panel; }


			virtual void Prepare(GGEMain &main, resource::File &file);

			void updatemapping();

			slider::Blueprint	*Scroller;
			checkbox::Blueprint *TitleLabel;
			checkbox::Blueprint *TitleButton;
			Placeholder			*TitleButtonPlace;
			checkbox::Blueprint *DialogButton;
			Placeholder			*DialogButtonPlace;


			Element *Normal;
			Element *Active;
			Element *Moving;
			Element *Disabled;

			Element *NormalToActive;
			Element *NormalToDisabled;
			Element *ActiveToMoving;

			Element *ActiveToNormal;
			Element *DisabledToNormal;
			Element *MovingToActive;


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
			BorderDataResource *GetScrollingBorder(StyleMode style, bool &transition) const {
				return Get<BorderDataResource, 3>(style, transition);
			}
			animation::RectangularGraphic2DSequenceProvider *GetResizer(StyleMode style, bool &transition) const {
				return Get<animation::RectangularGraphic2DSequenceProvider, 4>(style, transition);
			}
			BorderDataResource *GetTitleBorder(StyleMode style, bool &transition) const {
				return Get<BorderDataResource, 5>(style, transition);
			}
			BorderDataResource *GetDialogButtonBorder(StyleMode style, bool &transition) const {
				return Get<BorderDataResource, 6>(style, transition);
			}
			resource::Sound *GetSound(StyleMode style, bool &type) const {
				return Get<resource::Sound, 7>(style, type);
			}
			BorderDataResource *GetOverlay(StyleMode style, bool &transition) const {
				return Get<BorderDataResource, 8>(style, transition);
			}
			Placeholder *GetResizerPlace(StyleMode style, bool &transition) const {
				return Get<Placeholder, 9>(style, transition);
			}
			Byte GetOpacity(StyleMode style) const {
				if(Mapping[style.from][style.to]) {
					return Mapping[style.from][style.to]->Opacity;
				}

				if(style.to!=Blueprint::Style_None) {
					if(Mapping[style.to][style.from]) {
						return Mapping[style.to][style.from]->Opacity;
					}
				}

				if(style.to!=Blueprint::Style_None) {
					if(Mapping[style.from][Blueprint::Style_None]) {

						return Mapping[style.from][Blueprint::Style_None]->Opacity;
					}
				}

				if(style.from!=widgets::Blueprint::Normal && style.to!=widgets::Blueprint::Normal) {
					if(Mapping[widgets::Blueprint::Normal][Blueprint::Style_None]) {
						return Mapping[widgets::Blueprint::Normal][Blueprint::Style_None]->Opacity;
					}
				}

				return 255;
			}
			Byte GetBGOpacity(StyleMode style) const {
				if(Mapping[style.from][style.to]) {
					return Mapping[style.from][style.to]->BGOpacity;
				}

				if(style.to!=Blueprint::Style_None) {
					if(Mapping[style.to][style.from]) {
						return Mapping[style.to][style.from]->BGOpacity;
					}
				}

				if(style.to!=Blueprint::Style_None) {
					if(Mapping[style.from][Blueprint::Style_None]) {

						return Mapping[style.from][Blueprint::Style_None]->BGOpacity;
					}
				}

				if(style.from!=widgets::Blueprint::Normal && style.to!=widgets::Blueprint::Normal) {
					if(Mapping[widgets::Blueprint::Normal][Blueprint::Style_None]) {
						return Mapping[widgets::Blueprint::Normal][Blueprint::Style_None]->BGOpacity;
					}
				}

				return 255;
			}

		protected:
			utils::SGuid scroller;
			utils::SGuid titlelabel;
			utils::SGuid titlebutton;
			utils::SGuid titlebuttonplace;
			utils::SGuid dialogbutton;
			utils::SGuid dialogbuttonplace;

			utils::SGuid normal;
			utils::SGuid active;
			utils::SGuid moving;
			utils::SGuid disabled;

			utils::SGuid normaltoactive;
			utils::SGuid normaltodisabled;
			utils::SGuid activetomoving;

			utils::SGuid activetonormal;
			utils::SGuid disabledtonormal;
			utils::SGuid movingtoactive;

			Element *Mapping[6][6];


		};
		
		template<>
		inline BorderDataResource *Blueprint::Element::Get<BorderDataResource, 1>() const {
			return OuterBorder;
		}
		template<>
		inline BorderDataResource *Blueprint::Element::Get<BorderDataResource, 2>() const {
			return InnerBorder;
		}
		template<>
		inline BorderDataResource *Blueprint::Element::Get<BorderDataResource, 3>() const {
			return ScrollingBorder;
		}
		template<>
		inline animation::RectangularGraphic2DSequenceProvider *Blueprint::Element::Get<animation::RectangularGraphic2DSequenceProvider, 4>() const {
			return Resizer;
		}
		template<>
		inline BorderDataResource *Blueprint::Element::Get<BorderDataResource, 5>() const {
			return TitleBorder;
		}
		template<>
		inline BorderDataResource *Blueprint::Element::Get<BorderDataResource, 6>() const {
			return DialogButtonBorder;
		}
		template<>
		inline resource::Sound *Blueprint::Element::Get<resource::Sound, 7>() const {
			return Sound;
		}
		template<>
		inline BorderDataResource *Blueprint::Element::Get<BorderDataResource, 8>() const {
			return Overlay;
		}
		template<>
		inline Placeholder *Blueprint::Element::Get<Placeholder, 9>() const {
			return ResizerPlace;
		}

	}
} }


#pragma warning(pop)
