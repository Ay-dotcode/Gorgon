#pragma once


#include "../Resource/ResourceBase.h"
#include "Definitions.h"
#include "../Resource/ResourceFile.h"
#include "../Engine/Font.h"
#include <map>
#include "../Engine/Animation.h"
#include "Checkbox/CheckboxBlueprint.h"



namespace gge { namespace widgets {

	class WidgetRegistryResource;

	class WidgetRegistry {
	public:

		WidgetRegistry() : Icons(icons), Pictures(pictures) {}

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
			CButtons() : Button(NULL) {
			}

			checkbox::Blueprint *Button;
		} Buttons;
		

		template<class T_>
		class CCollection {
			friend class WidgetRegistry;
		public:
			T_ &operator [](const std::string &key) {
				if(parent.count(key)) {
					return parent.find(key)->second;
				}
				else {
					return parent.find("")->second;
				}
			}
		protected:
			CCollection(std::map<std::string, T_&> &parent) : parent(parent) { }

			std::map<std::string, T_&> &parent;
		};

		class CIcons : public CCollection<animation::RectangularGraphic2DSequenceProvider> {
			friend class WidgetRegistry;
		public:

			//this creates a new animation and you are responsible to delete it, 
			//use .DeleteAnimation to delete the object safely
			animation::RectangularGraphic2DAnimation &operator () (const std::string &key, bool create=true) {
				return this->operator [](key).CreateAnimation(create);
			}

			//this creates a new animation and you are responsible to delete it, 
			//use .DeleteAnimation to delete the object safely
			animation::RectangularGraphic2DAnimation &operator () (const std::string &key, animation::AnimationTimer &controller, bool owner=false) {
				return this->operator [](key).CreateAnimation(controller, owner);
			}

		protected:
			CIcons(std::map<std::string, animation::RectangularGraphic2DSequenceProvider&> &parent) : CCollection(parent) { }
		} Icons, Pictures;


	protected:
		std::map<std::string, animation::RectangularGraphic2DSequenceProvider&> icons;
		std::map<std::string, animation::RectangularGraphic2DSequenceProvider&> pictures;
	};

	WidgetRegistryResource *LoadWR(resource::File& File, std::istream &Data, int Size);

	class WidgetRegistryResource : public resource::ResourceBase {
		friend WidgetRegistryResource *LoadWR(resource::File& File, std::istream &Data, int Size);
	public:

		resource::File *file;
		

		virtual GID::Type getGID() const {
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