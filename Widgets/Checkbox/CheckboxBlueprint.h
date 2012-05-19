#pragma once

#pragma warning(push)
#pragma warning(disable:4351)

#include "../../Utils/Collection.h"
#include "../../Utils/SGuid.h"
#include "../Base/Blueprint.h"
#include "../Basic/BorderData.h"
#include "../../Engine/Font.h"
#include "../Basic/Placeholder.h"
#include "../../Resource/Sound.h"
#include "../../Utils/Size2D.h"
#include "../../Resource/NullImage.h"
#include "../../Engine/Wave.h"
#include <map>



namespace gge { namespace widgets {

	//only service classes will be here
	namespace checkbox {

		class Blueprint;

		Blueprint *Load(resource::File& File, std::istream &Data, int Size);

		class Blueprint : public widgets::Blueprint, public resource::Base {
			friend Blueprint *Load(resource::File& File, std::istream &Data, int Size);
		public:

			typedef int StateType;

			struct StateMode {
				StateMode(StateType from=1, StateType to=0) : from(from), to(to)
				{ }

				StateType from : 4;
				StateType to   : 4;

				bool operator <(const StateMode &s) const {
					if(to<s.to)
						return true;
					else if(to>s.to)
						return false;
					else						
						return from<s.from;
				}

				bool operator ==(const StateMode &s) const {
					return from==s.from && to==s.to;
				}

				StateMode swap() {
					return StateMode(to,from);
				}
			};

			class Group;

			struct GroupMode {
				GroupMode(FocusMode focus=FocusMode(), StateMode state=StateMode()) : focus(focus), state(state)
				{ }

				GroupMode(FocusType focus_from, int state_from, FocusType focus_to=Focus_None, int state_to=0) : 
				focus(focus_from,focus_to), state(state_from,state_to)
				{ }

				GroupMode(const Group &g);

				FocusMode focus;
				StateMode state;

				bool operator <(const GroupMode &e) const {
					if(focus<e.focus)
						return true;
					else if(focus==e.focus)
						return state<e.state;
					else						
						return false;
				}
			};

			enum StateNumbers {
				Single=1,
				Dual,
				Tri
			};

			enum SizingMode {
				Full=0,
				Auto=1
			};

			enum LineContentType {
				Empty			= 0,
				Symbol			= 1,
				Text			= 2,
				Icon			= 4,
			};

			struct LineContents {
				LineContents(LineContentType First=Empty, LineContentType Second=Empty, LineContentType Third=Empty) :
					First(First), Second(Second), Third(Third)
				{ }

				LineContents(int v) {
					this->t=v;
				}

				union {
					struct {
						LineContentType First  : 4;
						LineContentType Second : 4;
						LineContentType Third  : 4;
					};
					int t;
				};
			};

			enum TransitionType {
				   NoTransition,
				StyleTransition,
				StateTransition,
				FocusTransition,
			};

			enum HeightType {
				SizedToContents,
				Fixed,
				MaximumAvailable
			};

			class Element;

			class Line : public resource::Base {
				friend Line *LoadLine(resource::File& File, std::istream &Data, int Size);
				friend class Blueprint;
			public:

				Line() : Border(NULL)
				{ }

				GID::Type GetGID() const { return GID::Checkbox_Line; }

				BorderDataResource *Border;
				LineContents		Contents;
				SizingMode			WidthMode;
				HeightType		    HeightMode;
				int					Height;
				Alignment::Type		Align;//!vertical component of the alignment is not used

				LineContentType		GetContent(int id) {
					if(id==0) {
						return Contents.First;
					}
					else if(id==1) {
						return Contents.Second;
					}
					else {
						return Contents.Third;
					}
				}


				virtual void Prepare(GGEMain &main, resource::File &file);

			protected:
				utils::SGuid border;
			};

			class Element : public resource::Base {
				friend Blueprint::Element *LoadElement(resource::File& File, std::istream &Data, int Size);
				friend class Blueprint;
			public:
				Element() : Symbol(NULL), Sound(NULL), Border(NULL), Overlay(NULL), Font(NULL),
					SymbolPlace(NULL), TextPlace(NULL), IconPlace(NULL),
					Duration(-1), Lines()
				{ }

				GID::Type GetGID() const { return GID::Checkbox_Element; }

				int Duration;

				BorderDataResource			*Border;
				resource::
					ResizableObjectProvider	*Symbol;
				gge::Font						*Font;
				Placeholder					*SymbolPlace;
				Placeholder					*TextPlace;
				Placeholder					*IconPlace;
				resource::Sound		*Sound;
				BorderDataResource			*Overlay;

				Line *Lines[3];

				template<class T_, int id>
				T_ *Get() const {
					throw std::runtime_error("No such variable");
				}


				virtual void Prepare(GGEMain &main, resource::File &file);


				~Element() {
					utils::CheckAndDelete(Font);
				}

			protected:
				utils::SGuid border;
				utils::SGuid symbol;
				FontInitiator font;
				utils::SGuid symbolplace;
				utils::SGuid textplace;
				utils::SGuid iconplace;
				utils::SGuid sound;
				utils::SGuid overlay;
				utils::SGuid lines[3];

			};

			class Group : public resource::Base {
				friend Group *LoadGroup(resource::File& File, std::istream &Data, int Size);
				friend class Blueprint;
			public:

				Group() : Mapping(),
					Normal(NULL), Hover(NULL), Down(NULL), Disabled(NULL),
					NormalToHover(NULL), NormalToDown(NULL), NormalToDisabled(NULL), HoverToDown(NULL),
					HoverToNormal(NULL), DownToNormal(NULL), DisabledToNormal(NULL), DownToHover(NULL)
				{ }

				GID::Type GetGID() const { return GID::Checkbox_Group; }


				FocusMode Focus;
				StateMode State;


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
			};

			Blueprint() : States(Single), Mapping()
			{
				DefaultSize=utils::Size(120,40);
			}

			virtual void Prepare(GGEMain &main, resource::File &file);




			AnimationInfo hasstyleanimation(FocusMode f, StateMode s, StyleMode style) const  {
				if(Mapping[GroupMode(f,s)]) {
					if(Mapping[GroupMode(f,s)]->Mapping[style.from][style.to])
						return AnimationInfo(Forward,Mapping[GroupMode(f,s)]->Mapping[style.from][style.to]->Duration);

					if(Mapping[GroupMode(f,s)]->Mapping[style.to][style.from])
						return AnimationInfo(Backward,Mapping[GroupMode(f,s)]->Mapping[style.to][style.from]->Duration);
				}

				return Missing;
			}

			AnimationInfo HasStyleAnimation(FocusMode focus, StateMode state, StyleMode style) const;
			AnimationInfo HasStateAnimation(StateMode state) const;
			AnimationInfo HasFocusAnimation(FocusMode focus) const;


			//Group of size 5 is required
			void GetAlternatives(Group** &groups, FocusMode focus, StateMode state) const;

			template<class T_, int id>
			T_ *Get(Group **groups, StyleMode style, TransitionType &type) const {
				for(int i=0;i<5 && groups[i];i++) {
					Blueprint::Group *group=groups[i];

					if(group->Mapping[style.from][style.to] && group->Mapping[style.from][style.to]->Get<T_,id>()) {
						if(style.to!=0)
							type=Blueprint::StyleTransition;
						else if(group->Focus.to!=Blueprint::Focus_None)
							type=Blueprint::FocusTransition;
						else if(group->State.to!=0)
							type=Blueprint::StateTransition;
						else
							type=Blueprint::NoTransition;

						return group->Mapping[style.from][style.to]->Get<T_,id>();
					}

					if(style.to!=Blueprint::Style_None) {
						if(group->Mapping[style.to][style.from] && group->Mapping[style.to][style.from]->Get<T_,id>()) {
							if(group->Focus.to!=Blueprint::Focus_None)
								type=Blueprint::FocusTransition;
							else if(group->State.to!=0)
								type=Blueprint::StateTransition;
							else
								type=Blueprint::StyleTransition;

							return group->Mapping[style.to][style.from]->Get<T_,id>();
						}
					}

					//from -> None

					if(style.to!=Blueprint::Style_None || style.from!=Blueprint::Normal) {
						if(group->Mapping[Blueprint::Normal][Blueprint::Style_None] && group->Mapping[Blueprint::Normal][Blueprint::Style_None]->Get<T_,id>()) {
							if(group->Focus.to!=Blueprint::Focus_None)
								type=Blueprint::FocusTransition;
							else if(group->State.to!=0)
								type=Blueprint::StateTransition;
							else
								type=Blueprint::NoTransition;

							return group->Mapping[Blueprint::Normal][Blueprint::Style_None]->Get<T_,id>();
						}
					}
				}

				return NULL;
			}

			template<class T_, int id>
			T_ *GetNoInverse(Group **groups, StyleMode style, TransitionType &type) const {
				for(int i=0;i<5 && groups[i];i++) {
					Blueprint::Group *group=groups[i];

					if(group->Mapping[style.from][style.to] && group->Mapping[style.from][style.to]->Get<T_,id>()) {
						if(style.to!=0)
							type=Blueprint::StyleTransition;
						else if(group->Focus.to!=Blueprint::Focus_None)
							type=Blueprint::FocusTransition;
						else if(group->State.to!=0)
							type=Blueprint::StateTransition;
						else
							type=Blueprint::NoTransition;

						return group->Mapping[style.from][style.to]->Get<T_,id>();
					}

					//from -> None
				}

				return NULL;
			}

			BorderDataResource *GetOuterBorder(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<BorderDataResource, 1>(groups, style, type);
			}
			resource::ResizableObjectProvider *GetSymbol(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<resource::ResizableObjectProvider, 2>(groups, style, type);
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
			Placeholder *GetIconPlace(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<Placeholder, 6>(groups, style, type);
			}
			resource::Sound *GetSound(Group **groups, StyleMode style, TransitionType &type) const {
				return GetNoInverse<resource::Sound, 7>(groups, style, type);
			}
			BorderDataResource *GetOverlay(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<BorderDataResource, 8>(groups, style, type);
			}
			Line *GetLine(int id, Group **groups, StyleMode style, TransitionType &type) const {
				if(id==1)
					return Get<Line, 11>(groups, style, type);
				else if(id==2)
					return Get<Line, 12>(groups, style, type);
				else
					return Get<Line, 13>(groups, style, type);
			}
			Line *GetLine1(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<Line, 11>(groups, style, type);
			}
			Line *GetLine2(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<Line, 12>(groups, style, type);
			}
			Line *GetLine3(Group **groups, StyleMode style, TransitionType &type) const {
				return Get<Line, 13>(groups, style, type);
			}



			StateNumbers States;


			virtual GID::Type GetGID() const { return GID::Checkbox; }

			//not really but being unable to use [] makes life miserable
			mutable std::map<GroupMode, Group*> Mapping;

		protected:


		};

		template<>
		inline BorderDataResource *Blueprint::Element::Get<BorderDataResource, 1>() const {
			return Border;
		}
		template<>
		inline resource::ResizableObjectProvider *Blueprint::Element::Get<resource::ResizableObjectProvider, 2>() const {
			return Symbol;
		}
		template<>
		inline gge::Font *Blueprint::Element::Get<gge::Font, 3>() const {
			return Font;
		}
		template<>
		inline Placeholder *Blueprint::Element::Get<Placeholder, 4>() const {
			return SymbolPlace;
		}
		template<>
		inline Placeholder *Blueprint::Element::Get<Placeholder, 5>() const {
			return TextPlace;
		}
		template<>
		inline Placeholder *Blueprint::Element::Get<Placeholder, 6>() const {
			return IconPlace;
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
		inline Blueprint::Line *Blueprint::Element::Get<Blueprint::Line, 11>() const {
			return Lines[0];
		}
		template<>
		inline Blueprint::Line *Blueprint::Element::Get<Blueprint::Line, 12>() const {
			return Lines[1];
		}
		template<>
		inline Blueprint::Line *Blueprint::Element::Get<Blueprint::Line, 13>() const {
			return Lines[2];
		}

	}

}}

#pragma warning(pop)
