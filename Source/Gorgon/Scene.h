#pragma once

#include "Utils/Assert.h"
#include "Event.h"
#include "ConsumableEvent.h"
#include "Main.h"

#include "Layer.h"
#include "Input/Layer.h"
#include "Input/Keyboard.h"
#include "Graphics/Layer.h"
#include "Containers/Hashmap.h"
#include "Window.h"
#include "Time.h"

#include <stdexcept>

namespace Gorgon {
    /// Can be used to identify scenes
    using SceneID = int;

    /// Used to denote there is no explicit scene id given to a
    /// scene.
    static constexpr SceneID NoSceneID = -1;

    /**
    * This class represents a scene in the game like menu screen
    * in game, post game, pause or different game modes. Scenes
    * can be swapped on the fly and some scenes can be linked to 
    * others. Scene class is abstract, it must be derived by
    * other classes in order to work.
    */
    class Scene {
        friend class SceneManager;
    public:

        /// Empty constructor. After this constructor, the scene
        /// cannot be activated.
        explicit Scene(SceneID id = NoSceneID, bool mouseinput = true) : id(id), mouseinput(mouseinput) {
            main.Add(graphics);
            if(mouseinput)
                main.Add(mouse);
        }

        /// Sets the parent layer so that the scene can be activated.
        explicit Scene(Gorgon::Layer &parent, SceneID id = NoSceneID, bool mouseinput = true) : Scene(id, mouseinput) {
            this->parent = &parent;
        }

        /// Whether this scene requires keyboard input.
        virtual bool RequiresKeyInput() const = 0;

        /// Returns the ID of the current scene
        SceneID GetID() const {
            return id;
        }

        /// Called when the scene receives a key
        virtual void KeyEvent(Gorgon::Input::Key, float) { }

        /// Fires whenever activation of this layer is completed.
        /// Will not be fired if the layer is already active and
        /// Activate function is called.
        Gorgon::Event<Scene> ActivatedEvent{*this};

        /// Fires whenever deactivation of this layer is completed.
        /// Will not be fired if the layer is already inactive and
        /// Deactivate function is called.
        Gorgon::Event<Scene> DeactivatedEvent{*this};

    protected:
        /// Called after activation
        virtual void activate() = 0;

        /// Called *before* deactivation
        virtual void deactivate() { }

        /// Scene should perform its frame based operations in this
        /// function
        virtual void doframe(unsigned int delta) = 0;
        
        /// This is called after doframe to perform rendering operation.
        /// It is possible to receive render without receiving doframe,
        /// thus they should not be co-dependent.
        virtual void render() = 0;
        
        /// Changes the boundaries of the main layer
        void setlayerbounds(const Geometry::Bounds &bounds) {
            main.SetBounds(bounds);
        }


        /// ID of the current scene
        SceneID id;

        /// Graphics layer that can be drawn on
        Gorgon::Graphics::Layer graphics;

        /// Mouse layer that can be used to receive mouse events.
        /// If the mouseinput parameter of the constructor is
        /// set to true, this layer will be attached, otherwise
        /// it will not be functional.
        Gorgon::Input::Layer    mouse;

    private:

        void activate_scene() {
            if(isactive) return;

            if(!parent)
                throw std::runtime_error("This scene has no parent layer");

            parent->Add(main);

            isactive = true;

            activate();

            ActivatedEvent();
        }

        void deactivate_scene() {
            if(!isactive) return;

            deactivate();

            if(main.HasParent())
                main.GetParent().Remove(main);

            isactive = false;

            DeactivatedEvent();
        }
        
        Gorgon::Layer *parent = nullptr;
        Gorgon::Layer  main;
        bool mouseinput = false;
        bool isactive = false;
    };

    /**
    * This class manages the scenes, allows swapping between them
    * and manages the input. Acts as a layer so that it can be placed
    * on another layer. Requires an input source event to redirect
    * input to scenes. If no scene that requires input is active,
    * this object will disable its keyboard input so that the keys
    * will be redirected to another event. As soon as a scene is
    * activated, this object will carry its event handler to the top.
    * If necessary, use ActivateKeyboard function to manually carry
    * event handler to the top.
    */
    class SceneManager : public Gorgon::Layer {
    public:
        /// Default constructor, supply a window for key events
        SceneManager(Gorgon::Window &window) : 
            event(&window.KeyEvent),
            inputtoken(ConsumableEvent<Window, Gorgon::Input::Key, float>::EmptyToken)
        {
            inputtoken = event->Register([this](Gorgon::Input::Key key, float amount) {
                if(active && active->RequiresKeyInput())
                    active->KeyEvent(key, amount);

                return true;
            });

            event->Disable(inputtoken);
        }

        SceneManager() :
            inputtoken(ConsumableEvent<Window, Gorgon::Input::Key, float>::EmptyToken)
        { }

        ~SceneManager() {
            scenes.Destroy();
        }

        /// Switches the current scene to the scene with the given id.
        /// Will throw std::runtime_error if scene is not found. If
        /// given id is NoSceneID, all scenes will be deactivated.
        void SwitchScene(SceneID scene) {
            if(active && active->GetID() == scene)
                return;

            Deactivate();

            if(scene == NoSceneID) {
                return;
            }

            if(!scenes.Exists(scene))
                throw std::runtime_error("Scene does not exist");

            active = &scenes[scene];
            active->activate_scene();
            if(active->RequiresKeyInput()) {
                ASSERT(event, "Scene requires key input, but no keysource is given.");

                if(event) {
                    event->Enable(inputtoken);
                    event->MoveToTop(inputtoken);
                }
            }

            ActivatedEvent(*active);
        }

        /// Deactivates all scenes
        void Deactivate() {
            if(active) {
                active->deactivate_scene();
            }

            active = nullptr;

            if(event)
                event->Disable(inputtoken);
        }

        /// Sets the source window for key events
        void SetKeyEventSource(Window &window) {
            if(event)
                event->Unregister(inputtoken);

            event = &window.KeyEvent;

            inputtoken = event->Register([this](Gorgon::Input::Key key, float amount) {
                if(active && active->RequiresKeyInput())
                    active->KeyEvent(key, amount);

                return true;
            });

            if(!active || !active->RequiresKeyInput())
                event->Disable(inputtoken);
        }
        
        /// Moves keyboard input event to the top if there is a layer
        /// that accepts keyboard input.
        void ActivateKeyboard() {
            if(active && active->RequiresKeyInput()) {
                if(event) {
                    event->Enable(inputtoken);
                    event->MoveToTop(inputtoken);
                }
            }
        }
        
        /// Runs the application by running the active scene, 
        /// progressing OS events and rendering mechanism. This
        /// function will run until Quit is called.
        void Run() {
            while(!quiting) {
                if(active) {
                    active->doframe(Time::DeltaTime());
                    active->render();
                }
                
                Gorgon::NextFrame();
            }
            
            SwitchScene(NoSceneID);
        }
        
        /// Quits the scene manager, returning the execution to the
        /// point where Run function is called. It allows current
        /// frame to be completed before quiting. It also deactives
        /// the current scene.
        void Quit() {
            quiting = true;
        }

        /// Creates a new scene using the given type and parameters.
        /// Scene class should always take parent layer first, then
        /// id and then the other parameters if they exists. When
        /// using this function, only specify the class as template
        /// parameter, the others will be resolved. If a scene with 
        /// the same id exists, it will be deleted. It is advisable
        /// to add new scenes at the start without activating any
        /// scene.
        ///
        /// @code
        /// enum SceneIDs {
        ///		SCENE_MENU,
        ///		SCENE_GAME
        /// };
        /// 
        /// //...
        ///
        /// class GameScene : public Scene {
        /// public:
        ///		GameScene(Gorgon::Layer &parent, SceneID id, int players);
        ///
        /// //...
        /// };
        ///
        /// //...
        ///
        /// //Create the game scene with a single player.
        /// manager.NewScene<GameScene>(SCENE_GAME, 1);
        /// 
        /// manager.SwitchScene(SCENE_GAME);
        ///
        /// @endcode
        template<class S_, class ... P_>
        S_ &NewScene(SceneID id, P_ && ... params) {
            auto *s = new S_(*this, id, std::forward<P_>(params)...);

            scenes.Add(id, s, true);

            return *s;
        }

        /// This event will be fired whenever a scene is activated.
        Gorgon::Event<SceneManager, Scene&> ActivatedEvent{*this};

    private:
        Gorgon::Containers::Hashmap<SceneID, Scene> scenes;
        Scene *active = nullptr;
        Gorgon::ConsumableEvent<Window, Input::Key, float> *event = nullptr;
        Gorgon::ConsumableEvent<Window, Input::Key, float>::Token inputtoken;
        bool quiting = false;
    };
}
