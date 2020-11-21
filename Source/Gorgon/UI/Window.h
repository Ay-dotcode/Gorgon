#pragma once

#include "../Utils/Assert.h"
#include "../Event.h"
#include "../Main.h"
#include "../Layer.h"
#include "../Input/Layer.h"
#include "../Input/Keyboard.h"
#include "../Graphics/Layer.h"
#include "../Window.h"
#include "LayerAdapter.h"

#include "WidgetContainer.h"

#include <stdexcept>


namespace Gorgon { namespace UI {

    /**
    * UI window allows programmers to create an OS window that will accept
    * widgets and has the ability to run by its own.
    */
    class Window : public Gorgon::Window, public Runner, public WidgetContainer {
    public:
        using Gorgon::Window::Window;

        Window() : Gorgon::Window() {
        }
        
        ~Window() {
            KeyEvent.Unregister(inputtoken);
            CharacterEvent.Unregister(chartoken);
            delete extenderlayer;
        }
        
        Window(Window &&other) : Gorgon::Window(std::move(other)), WidgetContainer(std::move(other)) {
            KeyEvent.Unregister(inputtoken);
            CharacterEvent.Unregister(chartoken);
            
            inputtoken = keyinit();
            chartoken  = charinit();
            
            delete extenderlayer;
            extenderlayer = other.extenderlayer;
            Add(extenderlayer);
            other.extenderlayer = other.layerinit();
        }
        
        Window &operator=(Window &&other) {
            other.KeyEvent.Unregister(other.inputtoken);
            other.CharacterEvent.Unregister(other.chartoken);
            
            KeyEvent.Unregister(inputtoken);
            CharacterEvent.Unregister(chartoken);
            
            Gorgon::Window::operator =(std::move(other));
            WidgetContainer::operator =(std::move(other));
            
            
            inputtoken = keyinit();
            chartoken  = charinit();
            
            delete extenderlayer;
            extenderlayer = other.extenderlayer;
            Add(extenderlayer);
            adapter.SetLayer(*extenderlayer);
            other.extenderlayer = other.layerinit();
            other.Add(other.extenderlayer);
            other.adapter.SetLayer(*other.extenderlayer);
            
            return *this;
        }
        
        virtual Geometry::Size GetInteriorSize() const override {
            return Gorgon::Window::GetSize();
        }
        
        virtual bool IsVisible() const override {
            return Gorgon::Window::IsVisible();
        }
        
        /// Closes the window, returning the execution to the
        /// point where Run function is called. It allows current
        /// frame to be completed before quiting.
        virtual void Quit() override {
            quiting = true;
        }

        virtual void Run() override {
            while(!quiting) {
                Gorgon::NextFrame();
            }

            Gorgon::Window::Close();
        }

        virtual void Step() override {
            Gorgon::NextFrame();
        }
        
        /// Window does not do any scrolling, thus cannot ensure visibility
        bool EnsureVisible(const UI::Widget &) override {
            return true;
        }
        
        virtual ExtenderRequestResponse RequestExtender(const Gorgon::Layer &self) override {
            return {true, &adapter, self.TranslateToTopLevel(), GetSize()};
        }
        
        /// The spacing should be left between widgets
        virtual int GetSpacing() const override;
        
        /// Returns the unit width for a widget. This size is enough to
        /// have a bordered icon. Widgets should be sized according to unit
        /// width and spacing. A single unit width would be too small for
        /// most widgets.
        virtual int GetUnitWidth() const override;
        
        /// Overrides default spacing and unitwidth
        void SetSizes(int spacing, int unitwidth) {
            this->spacing = spacing;
            this->unitwidth = unitwidth;
            issizesset = true;
        }
        
        /// Sets the unit size automatically. Full width will be at least
        /// given units wide. Returns remaining size.
        int AutomaticUnitSize(int spacing, int units = 6) {
            this->spacing   = spacing;
            this->unitwidth = ( GetInteriorSize().Width - spacing * (units-1) ) / units;
            
            return GetInteriorSize().Width - (this->unitwidth * units + this->spacing * (units-1));
        }
        
        /// Return to use default sizes
        void UseDefaultSizes() {
            issizesset = false;
        }
        
        using WidgetContainer::Add;
        using Gorgon::Window::Add;
        using Gorgon::Window::KeyEvent;
        using Gorgon::Window::CharacterEvent;

    protected:
        virtual Gorgon::Layer &getlayer() override {
            return *this;
        }
        
        virtual bool ResizeInterior(Geometry::Size size) override {
            Gorgon::Window::Resize(size);
            
            return GetSize() == size;
        }

        decltype(KeyEvent)::Token keyinit() {
            inputtoken = KeyEvent.Register([this](Input::Key key, float amount) {
                return WidgetContainer::KeyEvent(key, amount);
            });
            
            KeyEvent.NewHandler = [this]{
                RegisterOnce([this] {
                    this->KeyEvent.MoveToTop(this->inputtoken);
                });
            };

            return inputtoken;
        }

        decltype(CharacterEvent)::Token charinit() {
            chartoken = CharacterEvent.Register([this](Char c) {
                return WidgetContainer::CharacterEvent(c);
            });
            
            CharacterEvent.NewHandler = [this]{
                RegisterOnce([this] {
                    this->CharacterEvent.MoveToTop(this->chartoken);
                });
            };

            return chartoken;
        }
        
        Graphics::Layer *layerinit() {
            auto l = new Graphics::Layer;
            Add(l);
            adapter.SetLayer(*l);
            return l;
        }

        void added(Layer &l) override {
            long ind = children.FindLocation(extenderlayer);
            if(ind != -1) {
                children.MoveBefore(ind, children.GetSize());
            }
            
            Gorgon::Window::added(l);
        }

    private:
        bool quiting = false;
        LayerAdapter adapter;
        Graphics::Layer *extenderlayer = layerinit();

        decltype(KeyEvent)::Token inputtoken = keyinit(); //to initialize token after window got constructed
        decltype(CharacterEvent)::Token chartoken = charinit(); //to initialize token after window got constructed
        
        int spacing   = 0;
        int unitwidth = 0;
        bool issizesset = false;
    };
    
} }

