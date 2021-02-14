#pragma once

#include "../UI/ComponentStackWidget.h"
#include "../UI/TwoStateControl.h"
#include "../Property.h"
#include "Layerbox.h"

#include "Registry.h"
#include "Composer.h"

#include "../Graphics/Bitmap.h"

namespace Gorgon { namespace Widgets {
    
    /**
     * This class shows a color plane allowing user to select one of them. The selected color may
     * have a border according to the template. It is also possible to adjust how it will be 
     * presented: as a list or as a wheel. Finally, it will be possible to use add fixed colors to
     * be displayed in the list. ColorPlane works with RGBAf, which can be substituted for RGBA when
     * needed.
     */
    class ColorPlane : public ComponentStackComposer {
    public:
        using ColorType = Graphics::RGBAf;
        
        /// Color plane modes
        enum Mode {
            List,
        };
        
        /// Display density constants
        enum Density {
            Low,
            Medium,
            High,
            VeryHigh
        };
        
        explicit ColorPlane(Registry::TemplateType type = Registry::ColorPlane_Regular) : 
            ColorPlane(Registry::Active()[type]) 
        { }

        explicit ColorPlane(const UI::Template &temp);

        /// Returns the current color
        ColorType GetValue() const {
            return color;
        }
        
        /// Sets the color. If the color does not match any color exactly, nothing will be shown as
        /// selected.
        void SetValue(const ColorType &value) {
            if(color == value)
                return;
            
            color = value;
            Refresh();
            ChangedEvent(value);
        }
        
        /// Sets the color. If the color does not match any color exactly, nothing will be shown as
        /// selected.
        ColorPlane &operator =(const ColorType &value) {
            SetValue(value);
            
            return *this;
        }
        
        /// Returns the current color
        operator ColorType() const {
            return GetValue();
        }
        
        //Get/Set/Property Mode
        
        //Get/Set/Property Alpha
        
        
        
        //SetCellSize
        
        using ComponentStackComposer::Resize;
        
        virtual void Resize(const Geometry::Size &size) override;
        
        void Refresh();
        
        Event<ColorPlane, Graphics::RGBAf> ChangedEvent = Event<ColorPlane, Graphics::RGBAf>{*this};
        Event<ColorPlane> ClickedEvent = Event<ColorPlane>{*this};
        
        /// Huedensity changes the number of different hue values displayed
        PROPERTY_REFRESH_VN(ColorPlane, , Density, HueDensity, huedensity, Medium);
        
        /// LCDensity changes the number of different luminance chromacity pairs displayed
        PROPERTY_REFRESH_VN(ColorPlane, , Density, LCDensity, lcdensity, Medium);
        
    private:
        Density lcdensity  = Medium;
        Density huedensity = Medium;
        
        Geometry::Pointf grayscaleoffset = {0, 0};
        Geometry::Pointf colortableoffset = {0, 0};
        Geometry::Pointf halftableoffset = {0, 0};
        Geometry::Sizef  stride = {0, 0};
        
        const std::vector<int> *htbl = nullptr;
        const std::vector<std::pair<int, int>> *lctbl = nullptr;

        Graphics::Layer  displaylayer;
        Input::Layer     inputlayer;
        Graphics::Bitmap display;
        Graphics::Bitmap selection;
        
        Layerbox layerbox;
        
        ColorType color = Graphics::Color::Transparent;
    
    protected:
        void click(Geometry::Point location);
        
        static const std::vector<std::pair<int, int>> lcpairs5;
        static const std::vector<std::pair<int, int>> lcpairs7;
        static const std::vector<std::pair<int, int>> lcpairs9;
        static const std::vector<std::pair<int, int>> lcpairs13;
        
        static const std::vector<int> huetable6;
        static const std::vector<int> huetable8;
        static const std::vector<int> huetable12;
        static const std::vector<int> huetable24;
    };
    
    
} }
