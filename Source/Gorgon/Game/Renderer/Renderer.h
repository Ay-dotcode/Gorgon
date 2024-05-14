/*

Renderer.h
Generic rendering system. 
Provides function, not a class. 
It's ODR violation but still the Renderer function will be here. 

*/

#pragma once
#include <Gorgon/Containers/Collection.h>
#include <Gorgon/Game/Exceptions/Exception.h>
#include <Gorgon/Geometry/Bounds.h>
#include <Gorgon/Geometry/Point.h>
#include <Gorgon/Geometry/Size.h>
#include <Gorgon/Graphics/Animations.h>
#include <Gorgon/Graphics/Font.h>
#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/Graphics/Bitmap.h>
#include <Gorgon/Graphics/Texture.h>
#include <Gorgon/Struct.h>
#include <Gorgon/Utils/Logging.h>
#include <Gorgon/Widgets/Registry.h>
#include <functional>
#include <initializer_list>
#include <vector>

#ifndef RENDERER_H
#define RENDERER_H


namespace Gorgon::Game::Rendering {
    template<class map_type, class derived> 
    class base_tile_renderer {
        public:
        using MapType = map_type; 

        void Render() {
            static_cast<derived*>(this)->render_impl(); 
        }
        void Prepare() {
            static_cast<derived*>(this)->prepare_resources();
            static_cast<derived*>(this)->generate_drawables();
        }
        
        std::vector<map_type> GetMap() const {
            return map_list; 
        }

        void SetActiveMap(int ID) {
            map = map_list[ID]; 
        }

        void SetActiveMap(map_type& map) {
            if(map_list.Find(map) != map_list.end()) { this->map = map; return; }
            map_list.emplace_back(map);
            this->map = map; 
        }

        map_type GetActiveMap() {
            return map; 
        }

        void SetLayer(Graphics::Layer& layer) {
            target_layer = &layer;
        }

        base_tile_renderer(Graphics::Layer& target_layer, const std::initializer_list<map_type>& map_list) : map_list(map_list) , target_layer(&target_layer) {}
        base_tile_renderer(Graphics::Layer& target_layer, const std::vector<map_type>& map_list) : map_list(map_list) , target_layer(&target_layer) {}
        
        base_tile_renderer(const std::initializer_list<map_type>& map_list) : map_list(map_list) {}


        protected: 
        std::vector<Graphics::Bitmap*> resources; 
        std::vector<Graphics::TextureImage*> drawables; 

        std::unordered_map<int, int> objects; 
 
        std::vector<map_type> map_list;
        map_type map; 

        Graphics::Layer * target_layer; 

        void RepeatCyclic(int outer, int inner, std::function<void(int, int)> Fp) {
            for(int i{}; i < outer; i++) {
                for(int j{}; j < inner; j++) {
                    Fp(i, j); 
                }
            }
        }
    };

    template<class map_type>
    class StandardTileRenderer : public base_tile_renderer<map_type, StandardTileRenderer<map_type>> {
        public: 
        StandardTileRenderer(Graphics::Layer& target_layer, const std::initializer_list<map_type>& map_list) : Base(target_layer, map_list), prepared(false), drawable_ready(false) {}  
        StandardTileRenderer(Graphics::Layer& target_layer, const std::vector<map_type>& map_list) : Base(target_layer, map_list), prepared(false), drawable_ready(false) {}  
        
        StandardTileRenderer(const std::initializer_list<map_type>& map_list) : Base(map_list), prepared(false), drawable_ready(false) {}  
        
        using MapType = map_type; 

        private:
        bool prepared, drawable_ready; 
        using Type = StandardTileRenderer<map_type>; 
        using Base = base_tile_renderer<map_type, Type>; 
        friend class base_tile_renderer<map_type, StandardTileRenderer<map_type>>; 

        using Base::target_layer; 
        using Base::map; 
        using Base::RepeatCyclic; 

        void prepare_resources() {
            if(prepared) 
                return;
            auto tilesets = Base::map.GetTileSets();
            Base::resources.reserve(tilesets.size());
            for(const auto& tileset : tilesets) {
                Base::resources.push_back(new Graphics::Bitmap{});
                Base::resources.back()->Import(tileset.image.source); 
                Base::resources.back()->Prepare(); 
            } 
            Base::objects = Base::map.ObjectLayerOrder(); 
            prepared = true;
        }

        void generate_drawables() {
            if(drawable_ready)
                return; 
            if(!prepared)
                throw Exception::not_prepared("Images are not prepared."); 

            auto tilesets = Base::map.GetTileSets();
            for(int i{}; i < Base::resources.size(); i++) {
                Geometry::Size atlas_size {
                    tilesets[i].tilewidth, 
                    tilesets[i].tileheight
                };
                auto boundaries = Base::resources[i]->GenerateAtlasBounds(atlas_size, 0); 
                std::vector<Graphics::TextureImage> item = Base::resources[i]->CreateAtlasImages(boundaries);
                for(auto j: item) {
                    Base::drawables.push_back(new Graphics::TextureImage{std::move(j)}); 
                }
            }
            drawable_ready = true; 
        }

        void render_impl() {
            if(!drawable_ready) 
                throw Exception::not_ready("The renderer is not ready to render yet. Did you try calling \"Prepare()\"?"); 

            auto size = target_layer->GetTargetSize();
            auto layers = map.GetLayers(); 
            const auto W = size.Width, H = size.Height; 
            const auto TW = map.tilewidth, TH = map.tileheight;
            const auto MW = map.width, MH = map.height;

            for(const auto& layer : layers) {
                auto map_data = layer.map_data(); 
                Base::RepeatCyclic(MW, MH, [map_data, MW, TH, TW, this](int x, int y) {
                    auto gid = map_data[x + y * MW]; 
                    if(gid == 0) 
                        return; 
                    if(this->Base::drawables[gid - 1]->GetImageSize().Height == TH * 2) {
                        this->Base::drawables[gid - 1]->Draw(*Base::target_layer, x * TW, (y * TH) - TH); 
                        return; 
                    }
                    this->Base::drawables[gid - 1]->Draw(*Base::target_layer, x * TW, y * TH); 
                });
                const auto& obj_it = Base::objects.find(layer.id);
                if(obj_it != Base::objects.end()) {
                    render_object(obj_it->second); 
                }
            }
        }

        void render_object(int id) {
            auto group = Base::map.GetObjectGroupMap(); 
            group[id].ForEach([this](auto obj) {
                this->Base::drawables[obj.gid - 1]->Draw(*Base::target_layer, obj.x, obj.y - obj.height);
            }); 

            for (auto [key, val] : group) {
                if (val.previous_object_group_index == id) {
                    render_object(val.id);
                    break; 
                } 
            }
        }
    };

    template<class map_type>
    class IsometricTileRenderer : public base_tile_renderer<map_type, IsometricTileRenderer<map_type>> {
        public: 
        IsometricTileRenderer(Graphics::Layer& target_layer, const std::initializer_list<map_type>& map_list) : Base(target_layer, map_list), prepared(false), drawable_ready(false) {}
        IsometricTileRenderer(Graphics::Layer& target_layer, const std::vector<map_type>& map_list) : Base(target_layer, map_list), prepared(false), drawable_ready(false) {}
        
        IsometricTileRenderer(const std::initializer_list<map_type>& map_list) : Base(map_list), prepared(false), drawable_ready(false) {}
        
        using MapType = map_type; 

        private: 
        bool prepared, drawable_ready; 
        using Type = IsometricTileRenderer<map_type>; 
        using Base = base_tile_renderer<map_type, Type>; 
        friend class base_tile_renderer<map_type, IsometricTileRenderer<map_type>>; 
        
        using Base::target_layer; 
        using Base::map; 

        void prepare_resources() {
            if(prepared) 
                return;
            auto tilesets = Base::map.GetTileSets();
            Base::resources.reserve(tilesets.size());
            for(const auto& tileset : tilesets) {
                Base::resources.emplace_back();
                Base::resources.back->Import(tileset.image.source); 
                Base::resources.back->Prepare(); 
            } 
            prepared = true;
        }

        void generate_drawables() {
            if(drawable_ready)
                return; 
            if(!prepared)
                throw Exception::not_prepared("Images are not prepared."); 

            auto tilesets = Base::map.GetTileSets();
            for(int i{}; i < Base::resources.size(); i++) {
                Geometry::Size atlas_size {
                    tilesets[i].image.width / (tilesets[i].tilecount / tilesets[i].columns),
                    tilesets[i].image.height / tilesets[i].columns
                };
                auto boundaries = Base::resources[i]->GenerateAtlasBounds(
                    atlas_size, 
                    Geometry::Point{0, 0}
                );

                auto item = Base::resources[i]->CreateAtlasImages(boundaries);
                for(auto i : item) {
                    Base::drawables.emplace_back(std::move(i)); 
                }
            }
            drawable_ready = true; 
        }

        void render_impl() {
            if(!drawable_ready) 
                throw Exception::not_ready("The renderer is not ready. Did you try calling \"Ready()\"?"); 

            auto size = target_layer->GetTargetSize();
            auto layers = map.GetLayers(); 
            const auto W = size.Width, H = size.Height; 
            const auto TW = map.tilewidth, TH = map.tileheight;
            const auto MW = map.width, MH = map.height; 

            for(const auto& layer: layers) {
                auto map_data = layer.map_data();
                Base::RepeatCyclic(MW, MH,[=](int x, int y) {
                    auto gid = map_data[x + y * MW]; 
                    if(gid == 0)
                        return; 
                    Geometry::Point point {
                        (y - x) * (TW / 2) + (TW * MW / 2), 
                        (y + x) * (TH / 2)
                    }; 
                    Base::drawables[gid - 1]->Draw(*target_layer, point);
                });
                const auto& obj_it = Base::objects.find(layer.id);
                if(obj_it != Base::objects.end()) {
                    render_object(obj_it->second); 
                }
            }
            
        }

        void render_object(int id) {
            auto group = Base::map.GetObjectGroupMap(); 
            group[id].ForEach([this](auto obj) {
                this->Base::drawables[obj.gid - 1].Draw(*Base::target_layer, obj.x, obj.y - obj.height);
            }); 

            for (auto [key, val] : group) {
                if (val.previous_object_group_index == id) {
                    render_object(val.id);
                    break; 
                } 
            }
        }
    };

}




#endif // RENDERER_H