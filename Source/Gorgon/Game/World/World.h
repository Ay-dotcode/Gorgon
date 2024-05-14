#pragma once 

/*
 * World class will be a wrapper system that'll contain the scenes, items etc.  
 */

#include <Gorgon/Event.h>
#include <Gorgon/Game/Renderer/Renderer.h>
#include <Gorgon/Geometry/Point.h>
#include <Gorgon/Geometry/Size.h>
#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/Scene.h>
#include <Gorgon/UI.h>
#include <vector>



namespace Gorgon::Game {
    template<typename renderer>
    class Scene : public Gorgon::Scene {

        private: 
        renderer map_renderer; 
        bool is_ready; 

        protected:
        void doframe(unsigned int delta) override {
            OnUpdate(delta); 
        }

        virtual void render() override {
            graphics.Clear(); 
            map_renderer.Render();

            OnRender(graphics);
        }

        bool RequiresKeyInput() const override {
            return true;
        }

        void MouseMove(Geometry::Point location) {
            OnMouseMove(location); 
        }

        public: 
        Scene(Gorgon::SceneManager &manager, Gorgon::SceneID id, std::vector<typename renderer::MapType> map) : Gorgon::Scene(manager, id), is_ready(false), map_renderer(graphics, map), OnUpdate(this), OnRender(this), OnMouseMove(this) { 
            mouse.SetMove(this, &Scene::MouseMove);
            Prepare();
        }

        Geometry::Size GetTargetSize() {
            return graphics.GetTargetSize();
        }

        void Prepare() {
            if(is_ready)
                return; 
            map_renderer.SetActiveMap(0);
            map_renderer.Prepare(); 
        }

        Widgets::Panel& GetUI() {
            return ui; 
        }


        Event<Game::Scene<renderer>, unsigned int> OnUpdate;
        Event<Game::Scene<renderer>, Graphics::Layer&> OnRender;
        Event<Game::Scene<renderer>, Geometry::Point> OnMouseMove; 

    };  


    template <typename... renderers> 
    class World {
        private: 
        friend class Gorgon::SceneManager; 

        
        Gorgon::SceneManager& manager; 
        SceneID active_scene; 

        public: 
        World(Gorgon::SceneManager& manager_, renderers &&... renderer) : manager(manager_), active_scene(1) {
            int i = 1; 
            Gorgon::UI::Initialize();

            for(auto r : {renderer...}) {
                this->manager.template NewScene<Scene<decltype(r)>>(i++, r.GetMap());
            }

            manager.DestroyedEvent.Register([&]() {
                exit(0);
            });

            manager.SwitchScene(1);

        }



        template<typename renderer_type>
        Scene<renderer_type>& GetScene(const int& ID) {
            auto * scene = &manager.GetScene(ID);
            auto& ret = *dynamic_cast<Scene<renderer_type> *>(scene); 
            return ret;
        }         

        template<typename renderer_type>
        Scene<renderer_type>& GetActiveScene() {
            auto * scene = &manager.GetScene(active_scene);
            auto& ret = *dynamic_cast<Scene<renderer_type> *>(scene); 
            return ret;
        }

        void SwitchScene(SceneID id) {
            active_scene = id; 
            manager.SwitchScene(id); 
        }

    };
}