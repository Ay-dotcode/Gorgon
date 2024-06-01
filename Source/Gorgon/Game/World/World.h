#pragma once 

/*
 * World class will be a wrapper system that'll contain the scenes, items etc.  
 */

#include <Gorgon/Event.h>
#include <Gorgon/Game/Renderer/Renderer.h>
#include <Gorgon/Geometry/Point.h>
#include <Gorgon/Geometry/Size.h>
#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/Input.h>
#include <Gorgon/Input/Mouse.h>
#include <Gorgon/Scene.h>
#include <Gorgon/UI.h>
#include <Gorgon/Game/Pathfinding/AStar.h>
#include <Gorgon/Utils/Assert.h>
#include <vector>



namespace Gorgon::Game {
    template<typename renderer, typename generator = Pathfinding::AStar::PathFinder>
    class Scene : public Gorgon::Scene {

        private: 
        renderer map_renderer; 
        bool is_ready, bg_render; 
        generator path_finder; 

        protected:
        void doframe(unsigned int delta) override {
            OnUpdate(delta); 
        }

        virtual void render() override {
            graphics.Clear(); 
            if(bg_render)
                map_renderer.Render();


            OnRender(graphics);
        }

        bool RequiresKeyInput() const override {
            return true;
        }

        void MouseMove(Geometry::Point location) {
            OnMouseMove(location); 
        }

        void KeyEvent(Input::Key k_, float f_) override {
            OnKeyEvent(k_, f_); 
        }

        public: 
        Scene(Gorgon::SceneManager &manager, Gorgon::SceneID id, std::vector<typename renderer::MapType> map, bool remove_ui) : Gorgon::Scene(manager, id), is_ready(false), map_renderer(graphics, map), 
        OnUpdate(this), OnRender(this), OnMouseMove(this), OnMouseDown(this), OnKeyEvent(this), OnMouseUp(this), OnMouseClick(this) {
            if (remove_ui)
                ActivatedEvent.Register([&]() {
                    ui.Remove();
                });

            path_finder.SetSize({map_renderer.GetActiveMap().tilewidth, map_renderer.GetActiveMap().tileheight});
            Prepare();
        }

        Scene(Gorgon::SceneManager &manager, Gorgon::SceneID id, std::vector<typename renderer::MapType> map) : Gorgon::Scene(manager, id), is_ready(false), map_renderer(graphics, map), 
        OnUpdate(this), OnRender(this), OnMouseMove(this), OnMouseDown(this), OnKeyEvent(this), OnMouseUp(this), OnMouseClick(this) {
            path_finder.SetSize({map_renderer.GetActiveMap().tilewidth, map_renderer.GetActiveMap().tileheight});
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

            mouse.SetMove(this, &Scene::MouseMove);
            mouse.SetDown([&](Geometry::Point loc_){ OnMouseDown(loc_);}); 
            mouse.SetUp([&](Geometry::Point loc_) { OnMouseMove(loc_); });
            mouse.SetClick([&](Geometry::Point loc_) { OnMouseClick(loc_); });
        }

        Widgets::Panel& GetUI() {
            return ui; 
        }

        void SetBackgroundRender(bool status) {
            bg_render = status; 
        }

        bool GetBackgroundRender() {
            return bg_render; 
        }

        renderer& GetRenderer() {
            return map_renderer; 
        }

        generator& GetPathFinder() {
            return path_finder; 
        }
        

        Event<Game::Scene<renderer>, unsigned int> OnUpdate;
        Event<Game::Scene<renderer>, Graphics::Layer&> OnRender;
        Event<Game::Scene<renderer>, Geometry::Point> OnMouseMove; 
        Event<Game::Scene<renderer>, Geometry::Point> OnMouseDown; 
        Event<Game::Scene<renderer>, Geometry::Point> OnMouseUp; 
        Event<Game::Scene<renderer>, Geometry::Point> OnMouseClick; 
        Event<Game::Scene<renderer>, Input::Key, float> OnKeyEvent; 
        

    };  

    class EmptyInitializer {}; 
    
    template <typename... renderers> 
    class World {
        protected:
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

        World(Gorgon::SceneManager& manager_, SceneID starting_id, renderers &&... renderer) : manager(manager_), active_scene(starting_id) {
            int i = starting_id; 
            Gorgon::UI::Initialize();

            for(auto r : {renderer...}) {
                this->manager.template NewScene<Scene<decltype(r)>>(i++, r.GetMap());
            }

            manager.DestroyedEvent.Register([&]() {
                exit(0);
            });

            manager.SwitchScene(1);
        }

        explicit World(Gorgon::SceneManager& manager_) : manager(manager_), active_scene(NoSceneID) {
            Gorgon::UI::Initialize();
        }



        template<typename renderer_type>
        Scene<renderer_type>& GetScene(const int& ID) {
            auto * scene = &manager.GetScene(ID);
            auto& ret = *dynamic_cast<Scene<renderer_type> *>(scene); 
            return ret;
        }         

        template<typename renderer_type>
        Scene<renderer_type>& GetActiveScene() {
            ASSERT(active_scene != NoSceneID, "No active scene is set.");

            auto * scene = &manager.GetScene(active_scene);
            auto& ret = *dynamic_cast<Scene<renderer_type> *>(scene); 
            return ret;
        }

        template<typename renderer_type>
        void ExecuteForActiveScene(std::function<void(Scene<renderer_type>&)> function) {
            function(GetActiveScene<renderer_type>()); 
        }

        template<typename renderer_type>
        void ExecuteForScene(int SceneID, std::function<void(Scene<renderer_type>&)> function) {
            function(GetScene<renderer_type>(SceneID)); 
        }

        template<typename Renderer_, typename PathFinder_ = Pathfinding::AStar::PathFinder, typename... Args_>
        void NewScene(SceneID id, Renderer_&& class_, Args_&&... args) {
            manager.template NewScene<Scene<Renderer_, PathFinder_>>(id, class_.GetMap(), std::forward<Args_>(args)...);
        }

        void DeleteScene(SceneID id) {
            manager.DeleteScene(id); 
        }

        void SwitchScene(SceneID id) {
            active_scene = id; 
            manager.SwitchScene(id); 
        }

    };
}
