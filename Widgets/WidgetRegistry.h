#pragma once


#include "../Resource/Base.h"
#include "Definitions.h"
#include "../Resource/File.h"
#include "../Engine/Font.h"
#include <map>
#include "../Engine/Animation.h"
#include "Checkbox/CheckboxBlueprint.h"
#include "Textbox/TextboxBlueprint.h"
#include "Combobox/ComboboxBlueprint.h"
#include "Listbox/ListboxBlueprint.h"
#include "Slider/SliderBlueprint.h"
#include "Panel/PanelBlueprint.h"
#include "../Resource/NullImage.h"
#include "TabpanelBlueprint.h"
#include "../Utils/EventChain.h"



namespace gge { namespace widgets {

	class WidgetRegistryResource;

	class WidgetRegistry {
	public:

		WidgetRegistry() : Icons(icons), Pictures(pictures), Sounds(sounds),
			Button(), Label(), Textbox(), WidgetSpacing(5, 5),
			Checkbox(), RadioButton(), Listbox(), Slider(), Scrollbar(),
			Progressbar(), Panel(), Window(), Combobox(), LoadedEvent("Loaded", this)
		{}

		void SetWRR(WidgetRegistryResource &wrr);

		class CFonts {
		public:
			Font Normal;
			Font H1;
			Font H2;
			Font H3;
			Font Strong;
			Font Emphasize;
			Font Quote;
			Font Info;
			Font Small;
			Font Link;
			Font Required;
			Font Important;
			Font Error;
			Font Warning;
			Font Success;
			Font Tooltip;
			Font Fixed;
			Font Decorative;
		} Fonts;

		class CColors {
		public:
			graphics::RGBint Text;
			graphics::RGBint Forecolor;
			graphics::RGBint Background;
			graphics::RGBint Alternate;
			graphics::RGBint AlternateBackground;
			graphics::RGBint Highlight;
			graphics::RGBint Light;
			graphics::RGBint Dark;
			graphics::RGBint Saturated;
			graphics::RGBint Desaturated;
			graphics::RGBint Shadow;
		} Colors;

		//default
		checkbox::Blueprint *Button;
		class CButtons {
		public:
			CButtons() : Button(NULL),
				Dialog(NULL),
				Menu(NULL),
				Tool(NULL),
				Navigation(NULL),
				Large(NULL),
				Small(NULL),
				Browse(NULL),
				Symbol(NULL),
				Decorative(NULL)
			{
			}

			checkbox::Blueprint 
				*Button,
				*Dialog,
				*Menu,
				*Tool,
				*Navigation,
				*Large,
				*Small,
				*Browse,
				*Symbol,
				*Decorative
			;
		} Buttons;

		textbox::Blueprint *Textbox;
		class CTextboxes {
		public:
			CTextboxes() : Textbox(NULL),
				Numberbox(NULL),
				Small(NULL),
				Password(NULL),
				TextEdit(NULL),
				CodeEdit(NULL)
			{
			}

			textbox::Blueprint 
				*Textbox,
				*Numberbox,
				*Small,
				*Password,
				*TextEdit,
				*CodeEdit
			;
		} Textboxes;

		checkbox::Blueprint *Checkbox;
		checkbox::Blueprint *RadioButton;
		class CCheckboxes {
		public:
			CCheckboxes(): Checkbox(NULL) ,
				Radio(NULL),
				Toggle(NULL),
				More(NULL),
				MenuCheck(NULL),
				MenuRadio(NULL),
				Lock(NULL)
			{ }

			checkbox::Blueprint 
				*Checkbox,
				*Radio,
				*Toggle,
				*More,
				*MenuCheck,
				*MenuRadio,
				*Lock
			;
		} Checkboxes;

		checkbox::Blueprint *Label;
		class CLabels {
		public:
			CLabels() : Label(NULL),
				Title(NULL),
				DataCaption(NULL),
				Bold(NULL),
				Heading(NULL),
				Subheading(NULL),
				ListCaption(NULL),
				Required(NULL),
				Hint(NULL),
				Tooltip(NULL),
				Link(NULL)
			{
			}

			checkbox::Blueprint 
				*Label,
				*Title,
				*DataCaption,
				*Bold,
				*Heading,
				*Subheading,
				*ListCaption,
				*Required,
				*Hint,
				*Tooltip,
				*Link
				;

		} Labels;

		listbox::Blueprint *Listbox;
		class CListboxes {
		public:
			CListboxes() : Listbox(), Action(), Picture(), File(), FileView(),
				Folder(), DataTable(), Detailed(), Checklist(), Radiolist(),
				Edit()
			{ }

			listbox::Blueprint 
				*Listbox,
				*Action,
				*Picture,
				*File,
				*FileView,
				*Folder,
				*DataTable,
				*Detailed,
				*Checklist,
				*Radiolist,
				*Edit;

		} Listboxes;


		slider::Blueprint *Slider;
		slider::Blueprint *Scrollbar;
		slider::Blueprint *Progressbar;
		class CSliders {
		public:

			CSliders() : Slider(), Selectbar(), ImageSelector(), Named(),
				Radiobar(), NumberSpinner(), ItemSpinner()
			{ }

			slider::Blueprint 
				*Slider,
				*Selectbar,
				*ImageSelector,
				*Named,
				*Radiobar,
				*NumberSpinner,
				*ItemSpinner
			;
		} Sliders;
		class CScrollbars {
		public:
			CScrollbars() : Scrollbar(), Dialog(), Picture()
			{ }

			slider::Blueprint
				*Scrollbar,
				*Dialog,
				*Picture
			;
		} Scrollbars;
		class CProgressbars {
		public:

			CProgressbars() : Progressbar(), File(), Working(), Loading(),
				Download(), Percent(), Unknown()
			{ }

			slider::Blueprint
				*Progressbar,
				*File,
				*Working,
				*Loading,
				*Download,
				*Percent
			;

			animation::RectangularGraphic2DAnimationProvider *Unknown;

		} Progressbars;

		panel::Blueprint *Panel;
		panel::Blueprint *Window;
		class CPanels {
		public:

			CPanels() : Panel(), OverlayControls(), Window(), DialogWindow(),
				Toolbar(), Menubar(), ToolWindow(), SettingsWindow(), Tabpanel()
			{ }


			panel::Blueprint 
				*Panel,
				*OverlayControls,
				*Window,
				*DialogWindow,
				*Toolbar,
				*Menubar,
				*ToolWindow,
				*SettingsWindow,
				*Fullscreen
			;
			tabpanel::Blueprint *Tabpanel;
			tabpanel::Blueprint *Tooltab;
			tabpanel::Blueprint *Settingstab;
		} Panels;

		combobox::Blueprint *Combobox;

		class COthers {
		public:
			COthers() : Combobox(), Autocomplete(), JumpList()
			{ }

			combobox::Blueprint *Combobox;
			combobox::Blueprint *Autocomplete;
			combobox::Blueprint *JumpList;
		} Others;

		utils::Point WidgetSpacing;
		utils::Margins ShadowMargins;



		template<class T_>
		class Collection {
			friend class WidgetRegistry;
		public:
			virtual T_ &operator [](const std::string &key) {
				if(parent.count(key)) {
					return parent.find(key)->second;
				}
				else if(parent.count("")) {
					return parent.find("")->second;
				}
				else {
					throw std::runtime_error("Cannot find item and no fallback is supplied");
				}
			}

			bool Exists(const std::string &key) const { return parent.count(key)>0; }
		protected:
			Collection(std::map<std::string, T_&> &parent) : parent(parent) { }

			std::map<std::string, T_&> &parent;
		};

		class ImageCollection : public Collection<animation::RectangularGraphic2DSequenceProvider> {
			friend class WidgetRegistry;
		public:

			virtual animation::RectangularGraphic2DSequenceProvider &operator [](const std::string &key) {
				if(parent.count(key)) {
					return parent.find(key)->second;
				}
				else if(parent.count("")) {
					return parent.find("")->second;
				}
				else {
					return resource::NullImage::Get();
				}
			}

			//this creates a new animation and you are responsible to delete it, 
			//use .DeleteAnimation to delete the object safely
			animation::RectangularGraphic2DAnimation &operator () (const std::string &key, bool create=true) {
				return this->operator [](key).CreateAnimation(create);
			}

			//this creates a new animation and you are responsible to delete it, 
			//use .DeleteAnimation to delete the object safely
			animation::RectangularGraphic2DAnimation &operator () (const std::string &key, animation::Timer &controller, bool owner=false) {
				return this->operator [](key).CreateAnimation(controller, owner);
			}

		protected:
			ImageCollection(std::map<std::string, animation::RectangularGraphic2DSequenceProvider&> &parent) : Collection(parent) { }
		} Icons, Pictures;

		class SoundCollection : public Collection<resource::Sound> {
			friend class WidgetRegistry;
		public:
			//this creates a new animation and you are responsible to delete it, 
			//use .DeleteAnimation to delete the object safely
			sound::Wave &operator () (const std::string &key) {
				return *(this->operator [](key).CreateWave());
			}

			//this creates a new animation and you are responsible to delete it, 
			//use .DeleteAnimation to delete the object safely
			sound::Wave &operator () (const std::string &key, float maxdistance) {
				return *(this->operator [](key).Create3DWave(maxdistance));
			}

		protected:
			SoundCollection(std::map<std::string, resource::Sound&> &parent) : Collection(parent) { }
		} Sounds;


		utils::EventChain<WidgetRegistry> LoadedEvent;


	protected:
		std::map<std::string, animation::RectangularGraphic2DSequenceProvider&> icons;
		std::map<std::string, animation::RectangularGraphic2DSequenceProvider&> pictures;
		std::map<std::string, resource::Sound&> sounds;
	};

	WidgetRegistryResource *LoadWR(resource::File& File, std::istream &Data, int Size);

	class WidgetRegistryResource : public resource::Base {
		friend WidgetRegistryResource *LoadWR(resource::File& File, std::istream &Data, int Size);
	public:

		resource::File *file;
		

		virtual GID::Type GetGID() const {
			return GID::WR;
		}

		virtual void Prepare(GGEMain &main, resource::File &file);

		template<class T_>
		T_ &Get(int index) {
			return dynamic_cast<T_&>(Subitems[index]);
		}

		virtual void Resolve(resource::File &file);
	};


	extern WidgetRegistry WR;

} }