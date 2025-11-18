#pragma once 

/*
 * World class will be a wrapper system that'll contain the scenes, items etc.  
 */

#include <Gorgon/Event.h>
#include <Gorgon/Game/Map/TiledMap.h>
#include <Gorgon/Game/Pathfinding/Pathfinders.h>
#include <Gorgon/Game/Renderer/Renderer.h>
#include <Gorgon/Game/Renderer/Tiled/Renderer.h>
#include <Gorgon/Geometry/Point.h>
#include <Gorgon/Geometry/Size.h>
#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/Input.h>
#include <Gorgon/Input/Mouse.h>
#include <Gorgon/Scene.h>
#include <Gorgon/UI.h>
#include <Gorgon/Game/Pathfinding/AStar.h>
#include <Gorgon/Utils/Assert.h>
#include <Gorgon/Window.h>
#include <vector>



/* namespace Gorgon::Game {
    class Scene : public Gorgon::Scene {
        private:
        std::unique_ptr<Rendering::base_tile_renderer<Map::Tiled::Map, class derived>> map_renderer; 
        std::unique_ptr<Pathfinding::base_pathfinder> path_finder;
        bool is_ready, bg_render; 

        protected: 
        void doframe(unsigned int delta) override {
            OnUpdate(delta); 
        }

        virtual void render() override {
            graphics.Clear(); 
            if(bg_render)
                map_renderer->Render();

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

        void InitFunctionBinds() {
            mouse.SetMove(this, &Scene::MouseMove);
            mouse.SetDown([&](Geometry::Point loc_){ OnMouseDown(loc_);}); 
            mouse.SetUp([&](Geometry::Point loc_) { OnMouseMove(loc_); });
            mouse.SetClick([&](Geometry::Point loc_) { OnMouseClick(loc_); });
        }

        void RemoveUI() {
            ActivatedEvent.Register([&]() {
                ui.Remove();
            });
        }

        void InitPathfinder() {
            path_finder->SetSize({map_renderer->GetActiveMap().tilewidth, map_renderer->GetActiveMap().tileheight});
            for(auto grid : map_renderer->GetActiveMap().GetPassabilityLayer().data_to_grid()) {
                if(not grid.is_passable()) {
                    path_finder->AddBlock(grid.location); 
                }
            }
        }

        void Init(bool remove_ui, bool prepare, bool path_find) {
            if (remove_ui)
                RemoveUI(); 

            if (prepare)
                Prepare();

            if (path_find) {
                InitPathfinder(); 
            }


            InitFunctionBinds();
        }

        void InitZoomed(int factor, bool remove_ui, bool path_find) {
            if (remove_ui)
                RemoveUI(); 

            PrepareZoomed(factor);

            if (path_find) {
                InitPathfinder(); 
            }

            InitFunctionBinds();
        }

        public: 
        using renderer = Rendering::Tiled::StandardRenderer;

        template<typename Renderer_>
        Scene(Gorgon::SceneManager &manager, Gorgon::SceneID id, Renderer_&& renderer_) : Gorgon::Scene(manager, id), is_ready(false), map_renderer(std::move(renderer_)), 
        OnUpdate(this), OnRender(this), OnMouseMove(this), OnMouseDown(this), OnKeyEvent(this), OnMouseUp(this), OnMouseClick(this), bg_render(true) {
            map_renderer->SetLayer(graphics);
            Init(false, true, true); 
        }

        Scene(Gorgon::SceneManager &manager, Gorgon::SceneID id, renderer&& renderer_, bool remove_ui, bool prepare, bool set_pathfinder) : Gorgon::Scene(manager, id), is_ready(false),  
        OnUpdate(this), OnRender(this), OnMouseMove(this), OnMouseDown(this), OnKeyEvent(this), OnMouseUp(this), OnMouseClick(this), bg_render(true) {
            map_renderer->SetLayer(graphics);
            Init(remove_ui, prepare, set_pathfinder); 
        }

        Scene(Gorgon::SceneManager &manager, Gorgon::SceneID id, renderer&& renderer_, bool remove_ui) : Gorgon::Scene(manager, id), is_ready(false), map_renderer(std::move(renderer_)), 
        OnUpdate(this), OnRender(this), OnMouseMove(this), OnMouseDown(this), OnKeyEvent(this), OnMouseUp(this), OnMouseClick(this), bg_render(true) {
            map_renderer->SetLayer(graphics);
            Init(remove_ui, true, true); 
        }

        Scene(Gorgon::SceneManager &manager, Gorgon::SceneID id, renderer&& renderer_, bool remove_ui, int zoom_factor) : Gorgon::Scene(manager, id), is_ready(false), map_renderer(std::move(renderer_)), 
        OnUpdate(this), OnRender(this), OnMouseMove(this), OnMouseDown(this), OnKeyEvent(this), OnMouseUp(this), OnMouseClick(this), bg_render(true) {
            map_renderer->SetLayer(graphics);
            InitZoomed(zoom_factor, remove_ui, true); 
        }

        Scene(Gorgon::SceneManager &manager, Gorgon::SceneID id, renderer&& renderer_, int zoom_factor) : Gorgon::Scene(manager, id), is_ready(false), map_renderer(std::move(renderer_)), 
        OnUpdate(this), OnRender(this), OnMouseMove(this), OnMouseDown(this), OnKeyEvent(this), OnMouseUp(this), OnMouseClick(this), bg_render(true) {
            map_renderer->SetLayer(graphics);
            InitZoomed(zoom_factor, false, true); 
        }

        void ReinitPathfinder() {
            path_finder->ClearBlocks();
            InitPathfinder();  
        }


        Geometry::Size GetTargetSize() {
            return graphics.GetTargetSize();
        }

        void Prepare() {
            if(is_ready)
                return; 
            map_renderer->SetActiveMap(0);
            map_renderer->Prepare(); 
        }

        void PrepareZoomed(int factor) {
            if(is_ready)
                return; 
            map_renderer->SetActiveMap(0); 
            map_renderer->PrepareZoomed(factor); 
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

        Rendering::base_tile_renderer<Map::Tiled::Map, class derived>& GetRenderer() {
            return *map_renderer; 
        }

        Pathfinding::base_pathfinder& GetPathFinder() {
            return *path_finder; 
        }

        Event<Game::Scene, unsigned int> OnUpdate;
        Event<Game::Scene, Graphics::Layer&> OnRender;
        Event<Game::Scene, Geometry::Point> OnMouseMove; 
        Event<Game::Scene, Geometry::Point> OnMouseDown; 
        Event<Game::Scene, Geometry::Point> OnMouseUp; 
        Event<Game::Scene, Geometry::Point> OnMouseClick; 
        Event<Game::Scene, Input::Key, float> OnKeyEvent; 
    }; 

    class World {
        protected:
        friend class Gorgon::SceneManager; 

        Gorgon::SceneManager manager; 
        SceneID active_scene; 

        public: 
        template<typename... T_, typename... Param>
        World(Gorgon::SceneManager&& manager_, T_&&... renderers, Param&&... params) : manager(std::move(manager_)), active_scene(1) {
            int i = 1; 
            Gorgon::UI::Initialize(); 

            for(auto  r : {renderers...}) {
                manager.NewScene<Scene>(i++, r, std::forward<Param>(params)...);
            }

            manager.DestroyedEvent.Register([&]() {
                exit(0); 
            }); 
            manager.SwitchScene(1);
        }

        template<typename... T_, typename... Param>
        World(Gorgon::SceneManager&& manager_, SceneID starting_id, T_&&... renderers, Param&&... params) : manager(std::move(manager_)), active_scene(starting_id) {
            int i = starting_id; 
            Gorgon::UI::Initialize(); 

            for(auto  r : {renderers...}) {
                manager.NewScene<Scene>(i++, std::move(r), std::forward<Param>(params)...);
            }

            manager.DestroyedEvent.Register([&]() {
                exit(0); 
            }); 
            manager.SwitchScene(1);
        }

        explicit World(Gorgon::SceneManager&& manager_) : manager(std::move(manager_)), active_scene(NoSceneID) {
            Gorgon::UI::Initialize();
        }

        template<typename Renderer_, typename PathFinder_ = Pathfinding::AStar::PathFinder, typename... Args_>
        void NewScene(SceneID id, Renderer_&& class_, Args_&&... args) {
            manager.template NewScene<Scene>(id, std::move(class_), std::forward<Args_>(args)...);
        }

        void Run() {
            manager.Run() ;
        }

    }; 
} */

namespace Gorgon::Game::Template {
    template <typename, typename T>
    struct has_passability_layer {};
    
    template <typename C, typename Ret, typename... Args>
    struct has_passability_layer<C, Ret(Args...)> {
    private:
        typedef char YesType[1];
        typedef char NoType[2];

        template <typename T>
        static YesType& test(decltype(&T::GetPassabilityLayer));

        template <typename>
        static NoType& test(...);

    public:
        static const bool value = sizeof(test<C>(0)) == sizeof(YesType);
    };
    
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

        void InitFunctionBinds() {
            mouse.SetMove(this, &Scene::MouseMove);
            mouse.SetDown([&](Geometry::Point loc_){ OnMouseDown(loc_);}); 
            mouse.SetUp([&](Geometry::Point loc_) { OnMouseMove(loc_); });
            mouse.SetClick([&](Geometry::Point loc_) { OnMouseClick(loc_); });
        }

        void RemoveUI() {
            ActivatedEvent.Register([&]() {
                ui.Remove();
            });
        }

        void InitPathfinder() {
            if constexpr (has_passability_layer<typename renderer::MapType, Map::Tiled::Layer&()>::value) {
                path_finder.SetSize({map_renderer.GetActiveMap().tilewidth, map_renderer.GetActiveMap().tileheight});
                for(auto grid : map_renderer.GetActiveMap().GetPassabilityLayer().data_to_grid()) {
                    if(not grid.is_passable()) {
                        path_finder.AddBlock(grid.location); 
                    }
                }
            } else {
                ASSERT(false, "Your class needs to have an information on passability layer.");
            }

        }

        void Init(bool remove_ui, bool prepare, bool path_find) {
            if (remove_ui)
                RemoveUI(); 

            if (prepare)
                Prepare();

            if (path_find) {
                InitPathfinder(); 
            }


            InitFunctionBinds();
        }

        void InitZoomed(int factor, bool remove_ui, bool path_find) {
            if (remove_ui)
                RemoveUI(); 

            PrepareZoomed(factor);

            if (path_find) {
                InitPathfinder(); 
            }

            InitFunctionBinds();
        }

        public:
                
        Scene(Gorgon::SceneManager &manager, Gorgon::SceneID id, std::vector<typename renderer::MapType> map) : Gorgon::Scene(manager, id), is_ready(false), map_renderer(graphics, map), 
        OnUpdate(this), OnRender(this), OnMouseMove(this), OnMouseDown(this), OnKeyEvent(this), OnMouseUp(this), OnMouseClick(this), bg_render(true) {
            Init(false, true, true); 
        }

        Scene(Gorgon::SceneManager &manager, Gorgon::SceneID id, std::vector<typename renderer::MapType> map, bool remove_ui, bool prepare, bool set_pathfinder) : Gorgon::Scene(manager, id), is_ready(false), map_renderer(graphics, map), 
        OnUpdate(this), OnRender(this), OnMouseMove(this), OnMouseDown(this), OnKeyEvent(this), OnMouseUp(this), OnMouseClick(this), bg_render(true) {
            Init(remove_ui, prepare, set_pathfinder); 
        }

        Scene(Gorgon::SceneManager &manager, Gorgon::SceneID id, std::vector<typename renderer::MapType> map, bool remove_ui) : Gorgon::Scene(manager, id), is_ready(false), map_renderer(graphics, map), 
        OnUpdate(this), OnRender(this), OnMouseMove(this), OnMouseDown(this), OnKeyEvent(this), OnMouseUp(this), OnMouseClick(this), bg_render(true) {
            Init(remove_ui, true, true); 
        }

        Scene(Gorgon::SceneManager &manager, Gorgon::SceneID id, std::vector<typename renderer::MapType> map, bool remove_ui, int zoom_factor) : Gorgon::Scene(manager, id), is_ready(false), map_renderer(graphics, map), 
        OnUpdate(this), OnRender(this), OnMouseMove(this), OnMouseDown(this), OnKeyEvent(this), OnMouseUp(this), OnMouseClick(this), bg_render(true) {
            InitZoomed(zoom_factor, remove_ui, true); 
        }

        Scene(Gorgon::SceneManager &manager, Gorgon::SceneID id, std::vector<typename renderer::MapType> map, int zoom_factor) : Gorgon::Scene(manager, id), is_ready(false), map_renderer(graphics, map), 
        OnUpdate(this), OnRender(this), OnMouseMove(this), OnMouseDown(this), OnKeyEvent(this), OnMouseUp(this), OnMouseClick(this), bg_render(true) {
            InitZoomed(zoom_factor, false, true); 
        }

        void ReinitPathfinder() {
            path_finder.ClearBlocks();
            InitPathfinder();  
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

        void PrepareZoomed(int factor) {
            if(is_ready)
                return; 
            map_renderer.SetActiveMap(0); 
            map_renderer.PrepareZoomed(factor); 
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
        

        Event<Game::Template::Scene<renderer>, unsigned int> OnUpdate;
        Event<Game::Template::Scene<renderer>, Graphics::Layer&> OnRender;
        Event<Game::Template::Scene<renderer>, Geometry::Point> OnMouseMove; 
        Event<Game::Template::Scene<renderer>, Geometry::Point> OnMouseDown; 
        Event<Game::Template::Scene<renderer>, Geometry::Point> OnMouseUp; 
        Event<Game::Template::Scene<renderer>, Geometry::Point> OnMouseClick; 
        Event<Game::Template::Scene<renderer>, Input::Key, float> OnKeyEvent; 
        
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
        
        void Run() {
            manager.Run(); 
        }

    };
}
