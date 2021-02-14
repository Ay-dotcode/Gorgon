#pragma once

#include "../UI/Template.h"

namespace Gorgon { namespace Widgets {

    
    /**
    * This class stores templates for elements. Once a registry is active
    * all created widgets will use the specified registry for templates.
    */
    class Registry {
    public:
        
        /// This enum lists all possible template types. All registries
        /// should be able to provide a template for each time, even if
        /// the template is completely empty.
        enum TemplateType {
            Button_Regular,
            Button_Icon,
            Button_Dialog,
            
            Label_Regular,
            Label_Error,
            Label_Bold,
            Label_Title,
            Label_Subtitle,
            Label_Leading,
            
            Checkbox_Regular,
            Checkbox_Button,
            
            Radio_Regular,
            
            Inputbox_Regular,
            
            Panel_Regular,
            Panel_Blank,
            Panel_Top,
            Panel_Left,
            Panel_Bottom,
            Panel_Right,
            
            Window_Regular,
            Window_Dialog,
            
            Progress_Regular,
            
            Layerbox_Regular,
            Layerbox_Blank,
            
            Scrollbar_Horizontal,
            Scrollbar_Vertical,
            
            Listbox_Regular,
            
            Dropdown_Regular,
            
            ColorPlane_Regular,
            
            
            ///Do not use this value
            Max 
        };
        
        /// Default constructor
        explicit Registry(bool activate = false) : templates({}) {
            if(activate)
                Activate();
        }
        
        /// Destroys all stored templates
        virtual ~Registry() {
            for(auto t : templates) {
                delete t;
            }
        }
        
        /// Activates this registry to be used to provide templates
        void Activate() {
            active = this;
        }
        
        static Registry &Active() {
            ASSERT(active, "UI is not initialized.");
            
            return *active;
        }
        
        /// Returns the template for the requested type.
        const UI::Template &operator[](TemplateType type) {
            if(templates[type] == nullptr) {
                templates[type] = &generate(type);
            }
            
            return *templates[type];
        }
    
        /// The spacing should be left between widgets
        virtual int GetSpacing() const = 0;

        /// The size of EM space. Roughly the same size as the height of a character.
        virtual int GetEmSize() const = 0;
        
        /// Returns the unit width for a widget. This size is enough to
        /// have a bordered icon. Widgets should be sized according to unit
        /// width and spacing. A single unit width would be too small for
        /// most widgets. Multiple units can be obtained by GetUnitWidth(n)
        virtual int GetUnitWidth() const = 0;
        
        /// Returns the width for a n-sized widget. Generally, 1 is for icons,
        /// 2 is for numberbox, 3 can be used for buttons, labels, short textboxes
        /// 4 for textboxes, 6 is for checkboxes. Standard panels can contain
        /// 6 units and they are less than 7 units wide.
        int GetUnitWidth(int n) const {
            return n * GetUnitWidth() + (n-1) * GetSpacing();
        }

    protected:
        /// This function should return a template for the given type. Due to 
        /// being used in constructors you are not allowed to reject template type.
        /// If the generator is capable of generating a similar template, simply
        /// return that one instead of throwing (ie. return Panel_Regular instead
        /// of Panel_Top if Panel_Top is not supported). If that is not possible
        /// as well, return a template with a fixed error image/text.
        virtual UI::Template &generate(TemplateType type) = 0;
        
        /// Stores the templates. Mutable to allow late loading.
        mutable std::array<UI::Template*, Max> templates;
        

    private:
        static Registry *active; //initialized in UI.cpp
    };
    
    
    /**
    * This registry contains preset templates. You are required to set all the templates
    * otherwise, missing templates will be generated completely empty. 
    */
    class PresetRegistry : public Registry {
    public:
        
        /// Add the given template to the registry, transferring ownership. If a template
        /// with the same type exists, it will be deleted.
        void Add(TemplateType type, UI::Template &temp) {
            delete templates[type];
            
            templates[type] = &temp;
        }
        
        void SetSpacing(const int tspacing) {
            spacing = tspacing;
        }

        void SetEmSize(const int size) {
            emsize = size;

        }

        virtual int GetSpacing()const override {
            return spacing;
        }

        virtual int GetEmSize()const override {
            return emsize;
        }

    protected:
        virtual UI::Template &generate(Gorgon::Widgets::Registry::TemplateType) override {
            return *new UI::Template();
        }
        
        int spacing = 5;

        int emsize = 14;

    };
    
    
} }
