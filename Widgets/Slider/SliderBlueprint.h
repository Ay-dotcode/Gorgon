#pragma once


#pragma warning(push)
#pragma warning(disable:4351)


#include "..\Base\BluePrint.h"
#include "..\Definitions.h"
#include "..\..\Resource\Sound.h"

#include <map>
#include "..\Checkbox\CheckboxBlueprint.h"
#include "..\..\Resource\Base.h"



namespace gge { namespace widgets {

	namespace slider {

		class Blueprint : public widgets::Blueprint, public resource::Base {
			friend Blueprint *Load(resource::File& File, std::istream &Data, int Size);
		public:


			enum OrientationType {
				Vertical = 0,
				Horizontal	,
				Bottom		,
				Right		,
				Top			,
				Left		,
			};

			enum TransitionType {
				NoTransition,
				StyleTransition,
				FocusTransition,
			};

			class Group;

			struct GroupMode {
				GroupMode(OrientationType orientation, FocusMode focus=FocusMode()) : focus(focus), orientation(orientation)
				{ }

				GroupMode(OrientationType orientation, FocusType focus_from, FocusType focus_to=Focus_None) : 
				focus(focus_from,focus_to), orientation(orientation)
				{ }

				GroupMode(const Group &g);

				FocusMode focus;
				OrientationType orientation;

				bool operator <(const GroupMode &e) const {
					if(orientation<e.orientation)
						return true;
					else if(orientation==e.orientation)
						return focus<e.focus;
					else
						return false;
				}
			};

			class Element : public resource::Base {
				friend Blueprint::Element *LoadElement(resource::File& File, std::istream &Data, int Size);
				friend class Blueprint;
			public:
				Element() : Symbol(NULL), Sound(NULL), Border(NULL), Overlay(NULL), Font(NULL),
					SymbolPlace(NULL), Duration(-1)
				{ }

				GID::Type GetGID() const { return GID::Slider_Element; }

				int Duration;

				BorderDataResource			*Border;
				animation::RectangularGraphic2DSequenceProvider	*Symbol;
				gge::Font					*Font;
				gge::Font					*ValueFont;
				Placeholder					*SymbolPlace;
				Placeholder					*TextPlace;
				Placeholder					*ValuePlace;
				resource::Sound		*Sound;
				BorderDataResource			*Overlay;

				BorderDataResource			*Ruler;
				BorderDataResource			*Indicator;
				BorderDataResource			*RulerOverlay;

				animation::RectangularGraphic2DSequenceProvider	*Tickmark;
				Placeholder					*TickmarkPlace;
				BorderDataResource			*TickmarkBorder;


				template<class T_, int id>
				T_ *Get() const {
					throw std::runtime_error("No such variable");
				}

				template<>
				BorderDataResource *Get<BorderDataResource, 1>() const {
					return Border;
				}
				template<>
				animation::RectangularGraphic2DSequenceProvider *Get<animation::RectangularGraphic2DSequenceProvider, 2>() const {
					return Symbol;
				}
				template<>
				gge::Font *Get<gge::Font, 3>() const {
					return Font;
				}
				template<>
				Placeholder *Get<Placeholder, 4>() const {
					return SymbolPlace;
				}
				template<>
				Placeholder *Get<Placeholder, 5>() const {
					return TextPlace;
				}
				template<>
				resource::Sound *Get<resource::Sound, 7>() const {
					return Sound;
				}
				template<>
				BorderDataResource *Get<BorderDataResource, 8>() const {
					return Overlay;
				}
				template<>
				BorderDataResource *Get<BorderDataResource, 14>() const {
					return Ruler;
				}
				template<>
				BorderDataResource *Get<BorderDataResource, 15>() const {
					return Indicator;
				}
				template<>
				BorderDataResource *Get<BorderDataResource, 16>() const {
					return RulerOverlay;
				}
				template<>
				animation::RectangularGraphic2DSequenceProvider *Get<animation::RectangularGraphic2DSequenceProvider, 17>() const {
					return Tickmark;
				}
				template<>
				Placeholder *Get<Placeholder, 18>() const {
					return TickmarkPlace;
				}
				template<>
				BorderDataResource *Get<BorderDataResource, 19>() const {
					return TickmarkBorder;
				}
				template<>
				Placeholder *Get<Placeholder, 20>() const {
					return ValuePlace;
				}
				template<>
				gge::Font *Get<gge::Font, 21>() const {
					return ValueFont;
				}

				virtual void Prepare(GGEMain &main, resource::File &file);


				~Element() {
					utils::CheckAndDelete(Font);
				}

			protected:
				utils::SGuid border;
				utils::SGuid symbol;
				FontInitiator font;
				FontInitiator valuefont;
				utils::SGuid symbolplace;
				utils::SGuid textplace;
				utils::SGuid valueplace;
				utils::SGuid sound;
				utils::SGuid overlay;

				utils::SGuid ruler;
				utils::SGuid indicator;
				utils::SGuid ruleroverlay;

				utils::SGuid tickmark;
				utils::SGuid tickmarkplace;
				utils::SGuid tickmarkborder;
			};

			class Group : public resource::Base {
				friend Group *LoadGroup(resource::File& File, std::istream &Data, int Size);
				friend class Blueprint;
			public:

				Group() : Mapping(),
					Normal(NULL), Hover(NULL), Down(NULL), Disabled(NULL),
					NormalToHover(NULL), NormalToDown(NULL), NormalToDisabled(NULL), HoverToDown(NULL),
					HoverToNormal(NULL), DownToNormal(NULL), DisabledToNormal(NULL), DownToHover(NULL),
					UpButton(NULL), DownButton(NULL), UpPlace(NULL), DownPlace(NULL)
				{ }

				GID::Type GetGID() const { return GID::Slider_Group; }


				FocusMode Focus;
				OrientationType Orientation;


				Element *Normal;
				Element *Hover;
				Element *Down;
				Element *Disabled;

				Element *NormalToHover;
				Element *NormalToDown;
				Element *NormalToDisabled;
				Element *HoverToDown;

				Element *HoverToNormal;
				Element *DownToNormal;
				Element *DisabledToNormal;
				Element *DownToHover;

				checkbox::Blueprint *UpButton;
				checkbox::Blueprint *DownButton;
				Placeholder *UpPlace;
				Placeholder *DownPlace;


				virtual void Prepare(GGEMain &main, resource::File &file);


				~Group() {
				}

				Element *Mapping[6][6];


			protected:
				utils::SGuid normal;
				utils::SGuid hover;
				utils::SGuid down;
				utils::SGuid disabled;

				utils::SGuid normaltohover;
				utils::SGuid normaltodown;
				utils::SGuid normaltodisabled;
				utils::SGuid hovertodown;

				utils::SGuid hovertonormal;
				utils::SGuid downtonormal;
				utils::SGuid disabledtonormal;
				utils::SGuid downtohover;

				utils::SGuid upbutton;
				utils::SGuid downbutton;
				utils::SGuid upplace;
				utils::SGuid downplace;
			};


			Blueprint() : Mapping(), TickSound(NULL)
			{
				DefaultSize=utils::Size(120,80);
			}


			AnimationInfo hasstyleanimation(OrientationType o, FocusMode f, StyleMode style) const  {
				if(Mapping[GroupMode(o, f)]) {
					if(Mapping[GroupMode(o, f)]->Mapping[style.from][style.to])
						return AnimationInfo(Forward,Mapping[GroupMode(o, f)]->Mapping[style.from][style.to]->Duration);

					if(Mapping[GroupMode(o, f)]->Mapping[style.to][style.from])
						return AnimationInfo(Backward,Mapping[GroupMode(o, f)]->Mapping[style.to][style.from]->Duration);
				}

				return Missing;
			}

			AnimationInfo HasStyleAnimation(OrientationType o, FocusMode focus, StyleMode style) const;
			AnimationInfo HasFocusAnimation(OrientationType o, FocusMode focus) const;

			//Group of size 3 is required
			void GetAlternatives(Group** &groups, OrientationType o, FocusMode focus) const;


			template<class T_, int id>
			T_ *Get(Group **groups, StyleMode style, TransitionType &type) const {
				for(int i=0;i<3 && groups[i];i++) {
					Blueprint::Group *group=groups[i];

					if(group->Mapping[style.from][style.to] && group->Mapping[style.from][style.to]->Get<T_,id>()) {
						if(style.to!=0)
							type=Blueprint::StyleTransition;
						else if(group->Focus.to!=Blueprint::Focus_None)
							type=Blueprint::FocusTransition;
						else
							type=Blueprint::NoTransition;

						return group->Mapping[style.from][style.to]->Get<T_,id>();
					}

					if(style.to!=Blueprint::Style_None) {
						if(group->Mapping[style.to][style.from] && group->Mapping[style.to][style.from]->Get<T_,id>()) {
							if(group->Focus.to!=Blueprint::Focus_None)
								type=Blueprint::FocusTransition;
							else
								type=Blueprint::StyleTransition;

							return group->Mapping[style.to][style.from]->Get<T_,id>();
						}
					}
				}

				for(int i=0;i<3 && groups[i];i++) {
					Blueprint::Group *group=groups[i];

					if(style.to!=Blueprint::Style_None || style.from!=Blueprint::Normal) {
						if(group->Mapping[Blueprint::Normal][Blueprint::Style_None] && group->Mapping[Blueprint::Normal][Blueprint::Style_None]->Get<T_,id>()) {
							if(group->Focus.to!=Blueprint::Focus_None)
								type=Blueprint::FocusTransition;
							else
								type=Blueprint::NoTransition;

							return group->Mapping[Blueprint::Normal][Blueprint::Style_None]->Get<T_,id>();
						}
					}
				}

				return NULL;
			}

			BorderDataResource *GetOuterBorder(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<BorderDataResource, 1>(groups, style, type);
			}
			animation::RectangularGraphic2DSequenceProvider *GetSymbol(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<animation::RectangularGraphic2DSequenceProvider, 2>(groups, style, type);
			}
			Font *GetFont(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<Font, 3>(groups, style, type);
			}
			Placeholder *GetSymbolPlace(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<Placeholder, 4>(groups, style, type);
			}
			Placeholder *GetTextPlace(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<Placeholder, 5>(groups, style, type);
			}
			resource::Sound *GetSound(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<resource::Sound, 7>(groups, style, type);
			}
			BorderDataResource *GetOverlay(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<BorderDataResource, 8>(groups, style, type);
			}
			BorderDataResource *GetRuler(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<BorderDataResource, 14>(groups, style, type);
			}
			BorderDataResource *GetIndicator(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<BorderDataResource, 15>(groups, style, type);
			}
			BorderDataResource *GetRulerOverlay(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<BorderDataResource, 16>(groups, style, type);
			}
			animation::RectangularGraphic2DSequenceProvider *GetTickmark(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<animation::RectangularGraphic2DSequenceProvider, 17>(groups, style, type);
			}
			Placeholder *GetTickmarkPlace(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<Placeholder, 18>(groups, style, type);
			}
			BorderDataResource *GetTickmarkBorder(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<BorderDataResource, 19>(groups, style, type);
			}
			Placeholder *GetValuePlace(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<Placeholder, 20>(groups, style, type);
			}
			Font *GetValueFont(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<Font, 21>(groups, style, type);
			}

			//For buttons and their places
			Group *GetOrientationBaseGroup(OrientationType o) const {
				return Mapping[GroupMode(o, NotFocused)];
			}

			resource::Sound *TickSound;


			virtual GID::Type GetGID() const { return GID::Slider; }

			//not really but being unable to use [] makes life miserable
			mutable std::map<GroupMode, Group*> Mapping;

			virtual void Prepare(GGEMain &main, resource::File &file);

		protected:
			utils::SGuid ticksound;


		};

	}
} }


#pragma warning(pop)
