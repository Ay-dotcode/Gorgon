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
            
            Label_Regular,
            Label_Error,
            
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
        
    protected:
        virtual UI::Template &generate(Gorgon::Widgets::Registry::TemplateType) override {
            return *new UI::Template();
        }
    };
    
    
} }
