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
        
        map_type GetMap() {
            return map;
        }

        base_tile_renderer(Graphics::Layer& target_layer, const map_type& map) : map(map), target_layer(target_layer) {}

        protected: 
        Containers::Collection<Graphics::Bitmap> resources; 
        std::vector<Graphics::TextureImage> drawables; 

        std::unordered_map<int, int> objects; 

        map_type map; 
        Containers::Collection<map_type> map_list;

        Graphics::Layer& target_layer; 

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
        StandardTileRenderer(Graphics::Layer& target_layer, const map_type& map) : Base(target_layer, map), prepared(false), drawable_ready(false) {}  
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
            Base::resources.Reserve(tilesets.size());
            for(const auto& tileset : tilesets) {
                Base::resources.AddNew();
                Base::resources.Last()->Import(tileset.image.source); 
                Base::resources.Last()->Prepare(); 
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
            for(int i{}; i < Base::resources.GetSize(); i++) {
                Geometry::Size atlas_size {
                    tilesets[i].tilewidth, 
                    tilesets[i].tileheight
                };
                auto boundaries = Base::resources[i].GenerateAtlasBounds(atlas_size, 0); 
                auto item = Base::resources[i].CreateAtlasImages(boundaries);
                for(auto i : item) {
                    Base::drawables.emplace_back(std::move(i)); 
                }
            }
            drawable_ready = true; 
        }

        void render_impl() {
            if(!drawable_ready) 
                throw Exception::not_ready("The renderer is not ready to render yet. Did you try calling \"Prepare()\"?"); 
            
            target_layer.Clear(); 

            auto size = target_layer.GetTargetSize();
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
                    if(this->Base::drawables[gid - 1].GetImageSize().Height == TH * 2) {
                        this->Base::drawables[gid - 1].Draw(Base::target_layer, x * TW, (y * TH) - TH); 
                        return; 
                    }
                    this->Base::drawables[gid - 1].Draw(Base::target_layer, x * TW, y * TH); 
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
                this->Base::drawables[obj.gid - 1].Draw(Base::target_layer, obj.x, obj.y - obj.height);
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
        IsometricTileRenderer(const map_type& map, Graphics::Layer& target_layer) : Base(map, target_layer), prepared(false), drawable_ready(false) {}
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
            Base::resources.Reserve(tilesets.size());
            for(const auto& tileset : tilesets) {
                Base::resources.AddNew();
                Base::resources.Last()->Import(tileset.image.source); 
                Base::resources.Last()->Prepare(); 
            } 
            prepared = true;
        }

        void generate_drawables() {
            if(drawable_ready)
                return; 
            if(!prepared)
                throw Exception::not_prepared("Images are not prepared."); 

            auto tilesets = Base::map.GetTileSets();
            for(int i{}; i < Base::resources.GetSize(); i++) {
                Geometry::Size atlas_size {
                    tilesets[i].image.width / (tilesets[i].tilecount / tilesets[i].columns),
                    tilesets[i].image.height / tilesets[i].columns
                };
                auto boundaries = Base::resources[i].GenerateAtlasBounds(
                    atlas_size, 
                    Geometry::Point{0, 0}
                );

                auto item = Base::resources[i].CreateAtlasImages(boundaries);
                for(auto i : item) {
                    Base::drawables.emplace_back(std::move(i)); 
                }
            }
            drawable_ready = true; 
        }

        void render_impl() {
            if(!drawable_ready) 
                throw Exception::not_ready("The renderer is not ready. Did you try calling \"Ready()\"?"); 
            
            Base::target_layer.Clear();

            auto size = target_layer.GetTargetSize();
            auto layers = map.GetLayers(); 
            const auto W = size.Width, H = size.Height; 
            const auto TW = map.tilewidth, TH = map.tileheight;
            const auto MW = map.width, MH = map.height; 

            for(const auto& layer: layers) {
                auto map_data = layer.map_data_2d(); 
                for(int x{}; x < MW; x++) {
                    for(int y{}; y < MH; y++) {
                        auto gid = map_data[x][y];
                        if(gid == 0)
                            continue;
                        Geometry::Point point {
                                (y - x) * (TW / 2) + (TW * MW / 2), 
                                (y + x) * (TH / 2)
                            };
                        if(point.X < W and point.Y < H)
                            Base::drawables[gid - 1].Draw(target_layer, point);
                    }
                }
            }
        }
    };

}




#endif // RENDERER_H