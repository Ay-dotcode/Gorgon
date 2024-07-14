/*

Renderer.h
Generic rendering system. 
Provides function, not a class. 
It's ODR violation but still the Renderer function will be here. 

*/

#pragma once
#include <Gorgon/CGI.h>
#include <Gorgon/CGI/Line.h>
#include <Gorgon/Containers/Collection.h>
#include <Gorgon/Game/Exceptions/Exception.h>
#include <Gorgon/Geometry/Bounds.h>
#include <Gorgon/Geometry/Point.h>
#include <Gorgon/Geometry/PointList.h>
#include <Gorgon/Geometry/Rectangle.h>
#include <Gorgon/Geometry/Size.h>
#include <Gorgon/Graphics/Animations.h>
#include <Gorgon/Graphics/Color.h>
#include <Gorgon/Graphics/Font.h>
#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/Graphics/Bitmap.h>
#include <Gorgon/Graphics/Texture.h>
#include <Gorgon/String.h>
#include <Gorgon/Struct.h>
#include <Gorgon/Utils/Assert.h>
#include <Gorgon/Utils/Logging.h>
#include <Gorgon/Widgets/Registry.h>
#include <bits/types/locale_t.h>
#include <functional>
#include <initializer_list>
#include <vector>

#ifndef RENDERER_H
#define RENDERER_H



namespace Gorgon::Game::Rendering {
    template <typename, typename T>
    struct has_bounds {};

    template <typename C, typename Ret, typename... Args>
    struct has_bounds<C, Ret(Args...)> {
    private:
        // Two types with different sizes
        typedef char YesType[1];
        typedef char NoType[2];

        // Check function
        template <typename T>
        static YesType& test(decltype(&T::bound_impl));

        template <typename>
        static NoType& test(...);

    public:
        static const bool value = sizeof(test<C>(0)) == sizeof(YesType);
    };

    template <typename, typename T>
    struct has_bounds_on {};

    template <typename C, typename Ret, typename... Args>
    struct has_bounds_on<C, Ret(Args...)> {
    private:
        typedef char YesType[1];
        typedef char NoType[2];

        template <typename T>
        static YesType& test(decltype(&T::bounds_on_impl));

        template <typename>
        static NoType& test(...);

    public:
        static const bool value = sizeof(test<C>(0)) == sizeof(YesType);
    };
    
    template <typename, typename T>
    struct has_pass_layer {};
    
    template <typename C, typename Ret, typename... Args>
    struct has_pass_layer<C, Ret(Args...)> {
    private:
        typedef char YesType[1];
        typedef char NoType[2];

        template <typename T>
        static YesType& test(decltype(&T::pass_layer_impl));

        template <typename>
        static NoType& test(...);

    public:
        static const bool value = sizeof(test<C>(0)) == sizeof(YesType);
    };
    
    template <typename, typename T>
    struct has_zoomed_prepare {};
    
    template <typename C, typename Ret, typename... Args>
    struct has_zoomed_prepare<C, Ret(Args...)> {
    private:
        typedef char YesType[1];
        typedef char NoType[2];

        template <typename T>
        static YesType& test(decltype(&T::prepare_zoomed_resources));

        template <typename>
        static NoType& test(...);

    public:
        static const bool value = sizeof(test<C>(0)) == sizeof(YesType);
    };
    
    template <typename, typename T>
    struct has_zoomed_generate {};
    
    template <typename C, typename Ret, typename... Args>
    struct has_zoomed_generate<C, Ret(Args...)> {
    private:
        typedef char YesType[1];
        typedef char NoType[2];

        template <typename T>
        static YesType& test(decltype(&T::generate_zoomed_drawables));

        template <typename>
        static NoType& test(...);

    public:
        static const bool value = sizeof(test<C>(0)) == sizeof(YesType);
    };

    class abstract_base_renderer {
        public: 
        virtual void Render(int, int) = 0; 
        virtual void Render(Geometry::Point) = 0; 
        virtual void BoundsOfTiles(Geometry::Point) = 0;
        virtual void BoundsOfTiles(CGI::StrokeSettings, CGI::SolidFill<>, Geometry::Point) = 0; 
        virtual Geometry::Point BoundsOnPoint(Geometry::Point, Geometry::Point) = 0; 
        virtual Geometry::Point BoundsOnPoint(Geometry::Point, CGI::StrokeSettings, CGI::SolidFill<>, Geometry::Point) = 0; 
        virtual std::shared_ptr<Graphics::TextureImage> GetImageByID(int) = 0; 
        virtual void PassabilityLayer() = 0; 
        virtual Geometry::Point TileCoordinateOnPoint(Geometry::Point) = 0; 
        virtual void Prepare() = 0; 
        virtual void PrepareZoomed(int) = 0;  
        virtual void Unprepare() = 0; 
        virtual void SetActiveMap(int, bool) = 0; 
        virtual void SetLayer(Graphics::Layer&) = 0; 
    }; 

    template<class map_type, class derived> 
    class base_tile_renderer  {
        public:
        using MapType = map_type; 
        using Derived = derived; 

        void Render(int offset_x = 0, int offset_y = 0) {
            ASSERT(target_layer != nullptr, "Layer is not set, you must set it first.");
            static_cast<derived*>(this)->render_impl(offset_x, offset_y); 
        }
        void Render(Geometry::Point offset) {
            ASSERT(target_layer != nullptr, "Layer is not set, you must set it first.");
            static_cast<derived*>(this)->render_impl(offset.X, offset.Y); 
        }
        void BoundsOfTiles(Geometry::Point offset = {0, 0}) {
            if constexpr (has_bounds<Derived, void(CGI::StrokeSettings, CGI::SolidFill<>, int, int)>::value)
                static_cast<derived*>(this)->bound_impl(1.0, CGI::SolidFill<>{Graphics::Color::Black}, offset.X, offset.Y); 
            else {
                ASSERT(false, "This function does not exist!");
            }
        }

        void BoundsOfTiles(CGI::StrokeSettings stroke, CGI::SolidFill<> color, Geometry::Point offset = {0, 0}) {
            if constexpr (has_bounds<Derived, void(CGI::StrokeSettings, CGI::SolidFill<>, int, int)>::value)
                static_cast<Derived*>(this)->bound_impl(stroke, color, offset.X, offset.Y); 
            else {
                ASSERT(false, "This function does not exist!");
            }
        }

        Geometry::Point BoundsOnPoint(Geometry::Point location, Geometry::Point offset = {0, 0}) {
            if constexpr (has_bounds_on<derived, Geometry::Point(Geometry::Point, CGI::StrokeSettings, CGI::SolidFill<>, int, int)>::value)
                return static_cast<Derived*>(this)->bounds_on_impl(location, 1.0, CGI::SolidFill<>{Graphics::Color::Black}, offset.X, offset.Y);
            else {
                ASSERT(false, "This function does not exist!");
            }
            return {-1, -1}; 
        }

        Geometry::Point BoundsOnPoint(Geometry::Point location, CGI::StrokeSettings stroke, CGI::SolidFill<> color, Geometry::Point offset = {0, 0}) {
            if constexpr (has_bounds_on<Derived, Geometry::Point(Geometry::Point, CGI::StrokeSettings, CGI::SolidFill<>, int, int)>::value)
                return static_cast<Derived*>(this)->bounds_on_impl(location, stroke, color, offset.X, offset.Y);
            else {
                ASSERT(false, "This function does not exist!");
            }
            return {-1, -1}; 
        }

        std::shared_ptr<Graphics::TextureImage> GetImageByID(int id) {
            return drawables[id];
        }

        void PassabilityLayer() {
            if constexpr (has_pass_layer<Derived, void()>::value) 
                return static_cast<Derived*>(this)->pass_layer_imp();
            else {
                ASSERT(false, "This function does not exist!"); 
            }
        }

        void Prepare() {
            static_cast<Derived*>(this)->prepare_resources();
            static_cast<Derived*>(this)->generate_drawables();
        }

        void Unprepare() {
            static_cast<Derived*>(this)->prepared = false; 
            static_cast<Derived*>(this)->drawable_ready = false; 
            resources.clear();
            drawables.clear();
        }

        void PrepareZoomed(int factor) {
            if constexpr(has_zoomed_prepare<Derived, void(int)>::value and has_zoomed_generate<Derived, void(int)>::value) {
                static_cast<Derived*>(this)->prepare_zoomed_resources(factor);
                static_cast<Derived*>(this)->generate_zoomed_drawables(factor);
            } else {
                ASSERT(false, "This function does not exist!"); 
            }

        }
        
        std::vector<map_type> GetMap() const {
            return map_list; 
        }

        void SetActiveMap(int ID, bool prepare = false) {
            map = map_list[ID]; 
            Unprepare(); 
            if(prepare) {
                this->Prepare(); 
            }
        }

        void SetActiveMap(map_type& map, bool prepare = false) {
            if(map_list.Find(map) != map_list.end()) { this->map = map; return; }
            map_list.emplace_back(map);
            this->map = map;
            static_cast<Derived*>(this)->prepared = false; 
            static_cast<Derived*>(this)->drawable_ready = false;
            resources.clear();
            drawables.clear();
            if(prepare) {
                this->Prepare(); 
            } 
        }

        map_type& GetActiveMap() {
            return map; 
        }

        void SetLayer(Graphics::Layer& layer) {
            target_layer = &layer;
        }

        base_tile_renderer(Graphics::Layer& target_layer, const std::initializer_list<map_type>& map_list) : map_list(map_list) , target_layer(&target_layer) {}
        base_tile_renderer(Graphics::Layer& target_layer, const std::vector<map_type>& map_list) : map_list(map_list) , target_layer(&target_layer) {}
        base_tile_renderer(const std::initializer_list<map_type>& map_list) : map_list(map_list), target_layer(nullptr) {}
        base_tile_renderer() : map_list(), target_layer(nullptr) {}

        protected: 
        std::vector<std::shared_ptr<Graphics::Bitmap>> resources;
        std::vector<std::shared_ptr<Graphics::TextureImage>> drawables;

        std::shared_ptr<Graphics::Bitmap> allbounds, singlebounds;

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
        StandardTileRenderer(Graphics::Layer& target_layer, const std::initializer_list<map_type>& map_list) : Base(target_layer, map_list), prepared(false), drawable_ready(false), first_time(true), first_time_all(true) {}  
        StandardTileRenderer(Graphics::Layer& target_layer, const std::vector<map_type>& map_list) : Base(target_layer, map_list), prepared(false), drawable_ready(false), first_time(true), first_time_all(true) {}  
        StandardTileRenderer(const std::initializer_list<map_type>& map_list) : Base(map_list), prepared(false), drawable_ready(false), first_time(true), first_time_all(true) {}  
        StandardTileRenderer() : Base(), prepared(false), drawable_ready(false), first_time(true), first_time_all(true) {}
        
        using MapType = map_type; 

        private:
        bool prepared, drawable_ready, first_time, first_time_all; 
        using Type = StandardTileRenderer<map_type>; 
        using Base = base_tile_renderer<map_type, Type>; 
        friend class base_tile_renderer<map_type, StandardTileRenderer<map_type>>; 

        friend class has_bounds_on<StandardTileRenderer<map_type>, Geometry::Point(Geometry::Point,CGI::StrokeSettings, CGI::SolidFill<>, int, int)>;
        friend class has_bounds<StandardTileRenderer<map_type>, void(CGI::StrokeSettings, CGI::SolidFill<>, int, int)>;
        friend class has_zoomed_prepare<StandardTileRenderer<map_type>, void(int)>;
        friend class has_zoomed_generate<StandardTileRenderer<map_type>, void(int)>;


        using Base::target_layer; 
        using Base::map; 
        using Base::RepeatCyclic; 
        
        void pass_layer_impl() {
            auto pass_layer = Base::map.GetPassabilityLayer(); 
            for (auto block : pass_layer.data_to_grid()) {
                if (block.is_passable()) { continue; }

                Geometry::Rectanglef tile {
                    block.location.X * Base::map.tilewidth, block.location.Y * map.tileheight,
                    Base::map.tilewidth, Base::map.tileheight
                };
                
                target_layer->Draw(tile, Graphics::RGBAf{Graphics::Color::Black});
            }
        }

        void prepare_zoomed_resources(int factor) {
            if(prepared)
                return;
            auto tilesets = Base::map.GetTileSets();
            Base::resources.reserve(tilesets.size());
            for(const auto& tileset : tilesets) {
                Graphics::Bitmap bmp;
                bmp.Import(tileset.image.source);
                Base::resources.push_back(std::make_shared<Graphics::Bitmap>(std::move(bmp.ZoomMultiple(factor))));
                Base::resources.back()->Prepare();
            }
            Base::objects = Base::map.ObjectLayerOrder();
            prepared = true;
        }

        void prepare_resources() {
            if(prepared) 
                return;
            auto tilesets = Base::map.GetTileSets();
            Base::resources.reserve(tilesets.size());
            for(const auto& tileset : tilesets) {
                Base::resources.push_back(std::make_shared<Graphics::Bitmap>());
                Base::resources.back()->Import(tileset.image.source); 
                Base::resources.back()->Prepare(); 
            } 
            Base::objects = Base::map.ObjectLayerOrder(); 
            prepared = true;
        }

        void generate_zoomed_drawables(int factor) {
            if(drawable_ready)
                return;
            if(not prepared)
                throw Exception::not_prepared("Images are not prepared");

            map.tilewidth *= factor;
            map.tileheight *= factor;

            for(int i{}; i < Base::resources.size(); i++) {
                map.GetTileSet(i).tilewidth  *= factor;
                map.GetTileSet(i).tileheight *= factor;
                Geometry::Size atlas_size {
                    map.GetTileSet(i).tilewidth,
                    map.GetTileSet(i).tileheight
                };

                auto boundaries = Base::resources[i]->GenerateAtlasBounds(atlas_size, 0);
                std::vector<Graphics::TextureImage> item = Base::resources[i]->CreateAtlasImages(boundaries);
                for(auto j : item) {
                    Base::drawables.push_back(std::make_shared<Graphics::TextureImage>(std::move(j)));
                }
            }
            drawable_ready = true;
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
                    Base::drawables.push_back(std::make_shared<Graphics::TextureImage>(std::move(j)));
                }
            }
            drawable_ready = true; 
        }

        void render_impl(int off_x = 0, int off_y = 0) {
            if(!drawable_ready) 
                throw Exception::not_ready("The renderer is not ready to render yet. Did you try calling \"Prepare()\"?"); 

            auto size = target_layer->GetTargetSize();
            auto layers = map.GetLayers(); 
            const auto W = size.Width, H = size.Height; 
            const auto TW = map.tilewidth, TH = map.tileheight;
            const auto MW = map.width, MH = map.height;

            for(const auto& layer : layers) {
                auto map_data = layer.map_data(); 
                auto name = Gorgon::String::ToLower(layer.name);
                if (name.find("passability") != -1) {
                    continue;
                }
                Base::RepeatCyclic(MW, MH, [&](int x, int y) {
                    auto gid = map_data[x + y * MW]; 
                    if(gid == 0) 
                        return; 
                    if(this->Base::drawables[gid - 1]->GetImageSize().Height == TH * 2) {
                        this->Base::drawables[gid - 1]->Draw(*Base::target_layer, x * TW + off_x, (y * TH) - TH + off_y); 
                        return; 
                    }
                    this->Base::drawables[gid - 1]->Draw(*Base::target_layer, x * TW + off_x, y * TH + off_y); 
                });
                const auto& obj_it = Base::objects.find(layer.id);
                if(obj_it != Base::objects.end()) {
                    render_object(obj_it->second, off_x, off_y); 
                }
            }
        }

        void render_object(int id, int off_x, int off_y) {
            auto group = Base::map.GetObjectGroupMap(); 
            group[id].ForEach([&](auto obj) {
                this->Base::drawables[obj.gid - 1]->Draw(*Base::target_layer, obj.x + off_x, obj.y - obj.height + off_y);
            }); 

            for (auto [key, val] : group) {
                if (val.previous_object_group_index == id) {
                    render_object(val.id, off_x, off_y);
                    break; 
                } 
            }
        }

        void bound_impl(CGI::StrokeSettings stroke, CGI::SolidFill<> color, int offset_x, int offset_y) {
            if(first_time_all) {
                Base::allbounds = std::make_shared<Graphics::Bitmap>();
                (*Base::allbounds).Resize(map.width * map.tilewidth, map.height * map.tileheight);
            
            
                Base::RepeatCyclic(Base::map.width, Base::map.height, [&](int x, int y) {
                    Geometry::PointList<> list; 
                    auto TW = map.tilewidth, TH = map.tileheight;
                    Geometry::Point top_left     = {x * TW, y * TH}, 
                                    bottom_left  = {x * TW, y * TH + TH},
                                    bottom_right = {x * TW + TW, y * TH + TH}, 
                                    top_right    = {x * TW + TW, y * TH}; 

                    list.Push(top_left);
                    list.Push(bottom_left);
                    list.Push(bottom_right);
                    list.Push(top_right);

                    CGI::DrawLines(*Base::allbounds, list, stroke, color);
                }); 

                first_time_all  = false; 
            }
            (*Base::allbounds).Prepare();
            (*Base::allbounds).Draw(*target_layer, 0 + offset_x, 0 + offset_y); 
        }
        
        Geometry::Point bounds_on_impl(Geometry::Point location, CGI::StrokeSettings stroke, CGI::SolidFill<> color, int offset_x, int offset_y) {
            if(first_time) {
                Base::singlebounds =  std::make_shared<Graphics::Bitmap>();
                (*Base::singlebounds).Resize(map.width * map.tilewidth, map.height * map.tileheight);
                first_time  = false; 
            }
            
            location.X -= offset_x; 
            location.Y -= offset_y; 

            auto TW = map.tilewidth, TH  = map.tileheight;
            int x = location.X / TW;
            int y = location.Y / TH; 

            Geometry::Point top_left     = {x * TW, y * TH}, 
                            bottom_left  = {x * TW, y * TH + TH},
                            bottom_right = {x * TW + TW, y * TH + TH}, 
                            top_right    = {x * TW + TW, y * TH}; 

            Geometry::PointList<> list; 
            list.Push(top_left);
            list.Push(bottom_left);
            list.Push(bottom_right);
            list.Push(top_right);
            list.Push(top_left);

            CGI::DrawLines(*Base::singlebounds, list, stroke, color);

            (*Base::singlebounds).Prepare();
            (*Base::singlebounds).Draw(*target_layer, offset_x, offset_y); 
            (*Base::singlebounds).Clear();

            return {x, y};
        }
    };

    template<class map_type>
    class IsometricTileRenderer : public base_tile_renderer<map_type, IsometricTileRenderer<map_type>> {
        public: 
        IsometricTileRenderer(Graphics::Layer& target_layer, const std::initializer_list<map_type>& map_list) : Base(target_layer, map_list), prepared(false), drawable_ready(false), first_time(true), first_time_all(true) {}
        IsometricTileRenderer(Graphics::Layer& target_layer, const std::vector<map_type>& map_list) : Base(target_layer, map_list), prepared(false), drawable_ready(false), first_time(true), first_time_all(true) {}
        IsometricTileRenderer(const std::initializer_list<map_type>& map_list) : Base(map_list), prepared(false), drawable_ready(false), first_time(true), first_time_all(true) {}
        IsometricTileRenderer() : Base(), prepared(false), drawable_ready(false) {}

        
        using MapType = map_type; 

        private: 
        bool prepared, drawable_ready, first_time, first_time_all; 
        using Type = IsometricTileRenderer<map_type>; 
        using Base = base_tile_renderer<map_type, Type>; 
        friend class base_tile_renderer<map_type, IsometricTileRenderer<map_type>>; 
        friend class has_bounds_on<IsometricTileRenderer<map_type>, Geometry::Point(Geometry::Point,CGI::StrokeSettings, CGI::SolidFill<>, int, int)>;
        friend class has_bounds<IsometricTileRenderer<map_type>, void(CGI::StrokeSettings, CGI::SolidFill<>, int, int)>;
        friend class has_zoomed_prepare<StandardTileRenderer<map_type>, void(int)>;
        friend class has_zoomed_generate<StandardTileRenderer<map_type>, void(int)>;

        using Base::target_layer; 
        using Base::map;
        
        void pass_layer_impl() {
            auto pass_layer = Base::map.GetPassabilityLayer(); 
            for (auto block : pass_layer.data_to_grid()) {
                if (block.is_passable()) { continue; }

                Geometry::Rectanglef tile {
                    block.location.X * Base::map.tilewidth, block.location.Y * map.tileheight,
                    Base::map.tilewidth, Base::map.tileheight
                };
                
                target_layer->Draw(tile, Graphics::RGBAf{Graphics::Color::Black});
            }
        }

        void prepare_zoomed_resources(float factor) {
            if(prepared)
                return;
            auto tilesets = Base::map.GetTileSets();
            Base::resources.reserve(tilesets.size());
            for(const auto& tileset : tilesets) {
                Graphics::Bitmap bmp;
                bmp.Import(tileset.image.source);
                Base::resources.push_back(std::make_shared<Graphics::Bitmap>(std::move(bmp.ZoomMultiple(factor))));
                Base::resources.back()->Prepare();
            }
            Base::objects = Base::map.ObjectLayerOrder();
            prepared = true;
        }

        void prepare_resources() {
            if(prepared) 
                return;
            auto tilesets = Base::map.GetTileSets();
            Base::resources.reserve(tilesets.size());
            for(const auto& tileset : tilesets) {
                Base::resources.push_back(std::make_shared<Graphics::Bitmap>());
                Base::resources.back()->Import(tileset.image.source); 
                Base::resources.back()->Prepare(); 
            } 
            prepared = true;
        }

        void generate_zoomed_drawables(float factor) {
            if(drawable_ready)
                return;
            if(not prepared)
                throw Exception::not_prepared("Images are not prepared");

            map.tilewidth *= factor;
            map.tileheight *= factor;

            for(int i{}; i < Base::resources.size(); i++) {
                map.GetTileSet(i).tilewidth  *= factor;
                map.GetTileSet(i).tileheight *= factor;
                Geometry::Size atlas_size {
                        map.GetTileSet(i).tilewidth,
                        map.GetTileSet(i).tileheight
                };

                auto boundaries = Base::resources[i]->GenerateAtlasBounds(atlas_size, 0);
                std::vector<Graphics::TextureImage> item = Base::resources[i]->CreateAtlasImages(boundaries);
                for(auto j : item) {
                    Base::drawables.push_back(std::make_shared<Graphics::TextureImage>(std::move(j)));
                }
            }
            drawable_ready = true;
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

                auto boundaries = Base::resources[i]->GenerateAtlasBounds(
                    atlas_size, 
                    Geometry::Point{0, 0}
                );

                auto item = Base::resources[i]->CreateAtlasImages(boundaries);
                for(auto i : item) {
                    Base::drawables.push_back(std::make_shared<Graphics::TextureImage>(std::move(i)));
                }
            }
            drawable_ready = true; 
        }

        void render_impl(int off_x = 0, int off_y = 0) {
            if(!drawable_ready) 
                throw Exception::not_ready("The renderer is not ready. Did you try calling \"Ready()\"?"); 

            auto size = target_layer->GetTargetSize();
            auto layers = map.GetLayers(); 
            const auto W = size.Width, H = size.Height; 
            const auto TW = map.tilewidth, TH = map.tileheight;
            const auto MW = map.width, MH = map.height; 

            for(const auto& layer: layers) {
                auto name = Gorgon::String::ToLower(layer.name);
            
                if (name.find("passability") != -1) {
                    continue;
                }
                auto map_data = layer.map_data();
                Base::RepeatCyclic(MW, MH,[=](int x, int y) {
                    auto active_layer = layer; 
                    auto gid = map_data[x + (y * MW)]; 
                    if(gid == 0)
                        return; 
                    Geometry::Point point {
                        (x - y) * (TW / 2) + (TW * MW / 2) + off_x, 
                        (y + x) * (TH / 2) + off_y
                    };  
                    Base::drawables[gid - 1]->Draw(*target_layer, point);
                });
                const auto& obj_it = Base::objects.find(layer.id);
                if(obj_it != Base::objects.end()) {
                    render_object(obj_it->second, off_x, off_y); 
                }
            }
            
        }

        void render_object(int id, int off_x, int off_y) {
            auto group = Base::map.GetObjectGroupMap(); 
            group[id].ForEach([&](auto obj) {
                this->Base::drawables[obj.gid - 1]->Draw(*Base::target_layer, obj.x + off_x, obj.y - obj.height + off_y);
            }); 

            for (auto [key, val] : group) {
                if (val.previous_object_group_index == id) {
                    render_object(val.id, off_x, off_y);
                    break; 
                } 
            }
        }

        void bound_impl(CGI::StrokeSettings stroke, CGI::SolidFill<> color, int offset_x, int offset_y) {
            if(first_time_all) {
                Base::allbounds = std::make_shared<Graphics::Bitmap>();
                (*Base::allbounds).Resize(map.width * map.tilewidth, map.height * map.tileheight);
            
                Base::RepeatCyclic(Base::map.width, Base::map.height, [&](int x, int y) {
                    Geometry::PointList<> list; 
                    
                    auto TW = map.tilewidth, TH = map.tileheight,
                         MW = map.width    , MH = map.height    ;


                Geometry::Point point {
                    (x - y) * (TW / 2) + (TW * MW / 2), 
                    (y + x) * (TH / 2) 
                }; 
                
                Geometry::Point top_left      = point, 
                                bottom_right  = {
                                    top_left.X + TW / 2.0f,
                                    top_left.Y + TH / 2.0f
                                },
                                top_right = {
                                    top_left.X - TW / 2.0f,
                                    top_left.Y + TH / 2.0f}, 
                                bottom_left    = {
                                    top_left.X, 
                                    top_left.Y + TH
                                }; 

                    list.Push(top_left);
                    list.Push(bottom_right);
                    list.Push(bottom_left);
                    list.Push(top_right);
                    list.Push(top_left);


                    CGI::DrawLines(*Base::allbounds, list, stroke, color);
                }); 

                first_time_all  = false; 
            }
            (*Base::allbounds).Prepare();
            (*Base::allbounds).Draw(*target_layer, 0 + offset_x, 0 + offset_y); 
        }
        
        Geometry::Point bounds_on_impl(Geometry::Point location, CGI::StrokeSettings stroke, CGI::SolidFill<> color, int offset_x, int offset_y) {
            if(first_time) {
                Base::singlebounds =  std::make_shared<Graphics::Bitmap>();
                (*Base::singlebounds).Resize(map.width * map.tilewidth, map.height * map.tileheight);
                first_time  = false; 
            }

            location.X -= offset_x; 
            location.Y -= offset_y;

            auto TW = map.tilewidth, TH = map.tileheight;
            auto MW = map.width    , MH = map.height; 

            float A = static_cast<float>(location.X - (TW * MW / 2.)) / (TW / 2.0) ;
            float B = static_cast<float>(location.Y) / (TH / 2.0);

            int x = static_cast<int>((A + B) / 2);
            int y = static_cast<int>((B - A) / 2);

            if(x < 0 or x > MW - 1 or y < 0 or y > MH - 1) {
                return {-1, -1}; 
            }


            Geometry::Point point {
                (x - y) * (TW / 2) + (TW * MW / 2), 
                (y + x) * (TH / 2) 
            }; 

            Geometry::Point top_left      = point,
                            bottom_right  = {
                                top_left.X + TW / 2.0f,
                                top_left.Y + TH / 2.0f
                            },
                            top_right = {
                                top_left.X - TW / 2.0f,
                                top_left.Y + TH / 2.0f}, 
                            bottom_left    = {
                                top_left.X, 
                                top_left.Y + TH
                            }; 

            Geometry::PointList<> list; 
                    list.Push(top_left);
                    list.Push(bottom_right);
                    list.Push(bottom_left);
                    list.Push(top_right);
                    list.Push(top_left);

            CGI::DrawLines(*Base::singlebounds, list, stroke, color);

            (*Base::singlebounds).Prepare();
            (*Base::singlebounds).Draw(*target_layer, offset_x, offset_y);
            (*Base::singlebounds).Clear();

            return {x, y};
        }
    };

}




#endif // RENDERER_H