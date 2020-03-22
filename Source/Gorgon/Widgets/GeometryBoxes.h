#pragma once

#include "Inputbox.h"
#include "../Geometry/Point.h"
#include "../Geometry/Size.h"
#include "../Geometry/Bounds.h"
#include "../Geometry/Margin.h"

namespace Gorgon { namespace Widgets {

    //These classes here are overloads to set default values.
    
    /// An inputbox variant designed to edit points.
    class Pointbox : public Inputbox<Geometry::Point> {
    public:
        using Inputbox::operator=;

        /// Initializes the inputbox
        explicit Pointbox(const UI::Template &temp, Geometry::Point value = {0, 0}) : Inputbox(temp, value) {
        }

        /// Initializes the inputbox
        explicit Pointbox(const UI::Template &temp, std::function<void()> changedevent) : Inputbox(temp, {0, 0}, changedevent) {
        }

        /// Initializes the inputbox
        explicit Pointbox(const UI::Template &temp, Geometry::Point value, std::function<void()> changedevent) : Inputbox(temp, value, changedevent) {
        }

        /// Initializes the inputbox
        explicit Pointbox(Geometry::Point value = {0, 0}, Registry::TemplateType type = Registry::Inputbox_Regular) : Inputbox(value, type) {
        }

        /// Initializes the inputbox
        explicit Pointbox(std::function<void()> changedevent, Registry::TemplateType type = Registry::Inputbox_Regular) : Inputbox({0, 0}, changedevent, type) {
        }

        /// Initializes the inputbox
        explicit Pointbox(Geometry::Point value, std::function<void()> changedevent, Registry::TemplateType type = Registry::Inputbox_Regular) : Inputbox(value, changedevent, type) {
        }
    };
    
    /// An inputbox variant designed to edit points with floating point coordinates.
    class Pointfbox : public Inputbox<Geometry::Pointf> {
    public:
        using Inputbox::operator=;

        /// Initializes the inputbox
        explicit Pointfbox(const UI::Template &temp, Geometry::Pointf value = {0, 0}) : Inputbox(temp, value) {
        }

        /// Initializes the inputbox
        explicit Pointfbox(const UI::Template &temp, std::function<void()> changedevent) : Inputbox(temp, {0, 0}, changedevent) {
        }

        /// Initializes the inputbox
        explicit Pointfbox(const UI::Template &temp, Geometry::Pointf value, std::function<void()> changedevent) : Inputbox(temp, value, changedevent) {
        }
        
        /// Initializes the inputbox
        explicit Pointfbox(Geometry::Pointf value = {0, 0}, Registry::TemplateType type = Registry::Inputbox_Regular) : Inputbox(value, type) {
        }

        /// Initializes the inputbox
        explicit Pointfbox(std::function<void()> changedevent, Registry::TemplateType type = Registry::Inputbox_Regular) : Inputbox({0, 0}, changedevent, type) {
        }

        /// Initializes the inputbox
        explicit Pointfbox(Geometry::Pointf value, std::function<void()> changedevent, Registry::TemplateType type = Registry::Inputbox_Regular) : Inputbox(value, changedevent, type) {
        }
    };

    
    /// An inputbox variant designed to edit size data.
    class Sizebox : public Inputbox<Geometry::Size> {
    public:
        using Inputbox::operator=;

        /// Initializes the inputbox
        explicit Sizebox(const UI::Template &temp, Geometry::Size value = {0, 0}) : Inputbox(temp, value) {
        }

        /// Initializes the inputbox
        explicit Sizebox(const UI::Template &temp, std::function<void()> changedevent) : Inputbox(temp, {0, 0}, changedevent) {
        }

        /// Initializes the inputbox
        explicit Sizebox(const UI::Template &temp, Geometry::Size value, std::function<void()> changedevent) : Inputbox(temp, value, changedevent) {
        }
        
        /// Initializes the inputbox
        explicit Sizebox(Geometry::Size value = {0, 0}, Registry::TemplateType type = Registry::Inputbox_Regular) : Inputbox(value, type) {
        }

        /// Initializes the inputbox
        explicit Sizebox(std::function<void()> changedevent, Registry::TemplateType type = Registry::Inputbox_Regular) : Inputbox({0, 0}, changedevent, type) {
        }

        /// Initializes the inputbox
        explicit Sizebox(Geometry::Size value, std::function<void()> changedevent, Registry::TemplateType type = Registry::Inputbox_Regular) : Inputbox(value, changedevent, type) {
        }
    };
    
    /// An inputbox variant designed to edit bounds data.
    class Boundsbox : public Inputbox<Geometry::Bounds> {
    public:
        using Inputbox::operator=;

        /// Initializes the inputbox
        explicit Boundsbox(const UI::Template &temp, Geometry::Bounds value = {0, 0, 0, 0}) : Inputbox(temp, value) {
        }

        /// Initializes the inputbox
        explicit Boundsbox(const UI::Template &temp, std::function<void()> changedevent) : Inputbox(temp, {0, 0, 0, 0}, changedevent) {
        }

        /// Initializes the inputbox
        explicit Boundsbox(const UI::Template &temp, Geometry::Bounds value, std::function<void()> changedevent) : Inputbox(temp, value, changedevent) {
        }
        
        /// Initializes the inputbox
        explicit Boundsbox(Geometry::Bounds value = {0, 0, 0, 0}, Registry::TemplateType type = Registry::Inputbox_Regular) : Inputbox(value, type) {
        }

        /// Initializes the inputbox
        explicit Boundsbox(std::function<void()> changedevent, Registry::TemplateType type = Registry::Inputbox_Regular) : Inputbox({0, 0, 0, 0}, changedevent, type) {
        }

        /// Initializes the inputbox
        explicit Boundsbox(Geometry::Bounds value, std::function<void()> changedevent, Registry::TemplateType type = Registry::Inputbox_Regular) : Inputbox(value, changedevent, type) {
        }
    };
    
    /// An inputbox variant designed to edit margin data.
    class Marginbox : public Inputbox<Geometry::Margin> {
    public:
        using Inputbox::operator=;

        /// Initializes the inputbox
        explicit Marginbox(const UI::Template &temp, Geometry::Margin value = {0, 0, 0, 0}) : Inputbox(temp, value) {
        }

        /// Initializes the inputbox
        explicit Marginbox(const UI::Template &temp, std::function<void()> changedevent) : Inputbox(temp, {0, 0, 0, 0}, changedevent) {
        }

        /// Initializes the inputbox
        explicit Marginbox(const UI::Template &temp, Geometry::Margin value, std::function<void()> changedevent) : Inputbox(temp, value, changedevent) {
        }
        
        /// Initializes the inputbox
        explicit Marginbox(Geometry::Margin value = {0, 0, 0, 0}, Registry::TemplateType type = Registry::Inputbox_Regular) : Inputbox(value, type) {
        }

        /// Initializes the inputbox
        explicit Marginbox(std::function<void()> changedevent, Registry::TemplateType type = Registry::Inputbox_Regular) : Inputbox({0, 0, 0, 0}, changedevent, type) {
        }

        /// Initializes the inputbox
        explicit Marginbox(Geometry::Margin value, std::function<void()> changedevent, Registry::TemplateType type = Registry::Inputbox_Regular) : Inputbox(value, changedevent, type) {
        }
    };
    
} }
