#pragma once

#include "../Utils/Assert.h"
#include "../Event.h"
#include "../Main.h"
#include "../Layer.h"
#include "../Input/Layer.h"
#include "../Input/Keyboard.h"
#include "../Graphics/Layer.h"
#include "../Window.h"

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
        
        Window(Window &&other) : Gorgon::Window(std::move(other)), WidgetContainer(std::move(other)) {
            KeyEvent.Unregister(inputtoken);
            CharacterEvent.Unregister(chartoken);
            
            inputtoken = keyinit();
            chartoken  = charinit();
        }
        
        Window &operator=(Window &&other) {
            Gorgon::Window::operator =(std::move(other));
            WidgetContainer::operator =(std::move(other));
            
            KeyEvent.Unregister(inputtoken);
            CharacterEvent.Unregister(chartoken);
            
            inputtoken = keyinit();
            chartoken  = charinit();
            
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
        bool EnsureVisible(const UI::WidgetBase &) override {
            return true;
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


    private:
		bool quiting = false;

		decltype(KeyEvent)::Token inputtoken = keyinit(); //to initialize token after window got constructed
		decltype(CharacterEvent)::Token chartoken = charinit(); //to initialize token after window got constructed
    };
    
} }

