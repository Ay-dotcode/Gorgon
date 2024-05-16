#pragma once 

#include <Gorgon/External/PugiXML/pugi/pugixml.hpp>
#include <Gorgon/Game/Exceptions/Exception.h>
#include <Gorgon/Game/Parse/FillerSystem/Filler.h>
#include <Gorgon/Geometry/Point.h>
#include <Gorgon/Geometry/Size.h>
#include <Gorgon/Graphics/Bitmap.h>
#include <Gorgon/Graphics/Texture.h>
#include <Gorgon/String.h>
#include <Gorgon/Struct.h>
#include <array>
#include <cstddef>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief This namespace containes example tile map clas, which is in use for parsing
 * the Tiled map files (.tmx). This class will be a part of the world class and can be
 * used for rendering. The generic renderer will be based on this example system and if 
 * you configure your class this to fit this shape, then you can also use the generic 
 * renderer. 
 */
namespace Gorgon::Game::Map::Tiled { 
    struct GridTile {
        public: 
        Gorgon::Geometry::Point location; 
        GridTile* parent; 
        float gCost, hCost, fCost; 
        bool passable; 
        GridTile() : parent(nullptr) {}
        GridTile(const Gorgon::Geometry::Point& location, bool passable) : location(location), parent(nullptr), gCost(0), hCost(0), passable(passable) {}

        void operator=(const GridTile& other) {
            this->parent = other.parent; 
            gCost = other.gCost; 
            hCost = other.hCost; 
            fCost = other.fCost; 
            passable = other.passable; 
            location = other.location; 
        }
        float f_cost() const {
            return (gCost + hCost);
        }  
        bool is_passable() {
            return passable; 
        }

        bool operator==(GridTile& other) {
            return (this->location == other.location);
        }
    }; 


    /**
     @brief Tiled object layer class
     * 
     */
    
    struct Object {
        public: 

        const static inline std::string tag = "object"; 

        int id, gid; 
        float x, y; 
        int width, height; 

        DefineStructMembers(Object, id, gid, x, y, width, height); 
    }; 
    
    struct ObjectGroup {
        public: 
        void Set(const pugi::xml_node_iterator& in) {
            auto objects  = (*in).parent().children("object"); 
            if(objects.empty()) {
                return; 
            }
            Parse::Filler::Fill(obj_list, objects); 
            auto upper = (*in).parent().parent();
            for(auto i {upper.begin()}; i != upper.end(); i++) {
                if((*i).attribute("id").value() != std::to_string(id)) {
                    continue;
                }
                if ((*i).previous_sibling().name() == std::string("layer")) {
                    previous_layer_index = std::stoi((*i).previous_sibling().attribute("id").value()); 
                    previous_object_group_index = -1; 
                    break;
                }
                if ((*i).previous_sibling().name() == tag) {
                    previous_layer_index = -2; 
                    previous_object_group_index = std::stoi((*i).previous_sibling().attribute("id").value());
                }

            }
        }

        const static inline std::string tag = "objectgroup"; 


        int id; 
        std::string name; 

        DefineStructMembers(ObjectGroup, id, name); 

        int previous_layer_index, previous_object_group_index; 

        Object operator[](size_t index) {
            return obj_list[index]; 
        }

        Object GetObject(size_t index) {
            return obj_list[index]; 
        }

        std::vector<Object> GetObjects() {
            return obj_list; 
        }

        void ForEach(std::function<void(Object)> fp) {
            for(auto obj : obj_list) {
                fp(obj); 
            }
        }

        private: 
        std::vector<Object> obj_list;  
    }; 

    /**
     * @brief Tiled, tileset class. 
     */
    struct TileSet {
        public:
        void Set(const pugi::xml_node_iterator& in) {
            image.source = (*in).attribute("source").value(); 
            image.height = std::stoi((*in).attribute("height").value());
            image.width  = std::stoi((*in).attribute("width").value());  
        }

        struct Image {
            std::string source; 
            int width, height; 
        };
        /**
         * @brief This has to be here for parsing. 
         * Fill function needs the tag to find the attributes and image source. 
         * It has to be const static since the Fill function it self is const 
         */
        const static inline std::string tag = "tileset"; 

        /**
         * @brief Attributes of the tileset node.
         * .tmx file contains these attributes in the tileset node. They are also required
         * for rendering.
         */
        int firstgid, tilewidth, tileheight, tilecount, columns; 
        std::string name; 
        TileSet() {}
        /**
         * @brief Reflection
         * The below macro call creates the Reflection part of the process. 
         */
        DefineStructMembers(TileSet, firstgid, tilewidth, tileheight, tilecount, columns, name); 
        /**
         * @brief Getting the source.
         * I couldn't find any better way to handle it yet. 
         */
        Image image; 


    };

    /**
     * @brief Tiled, layer class. 
     */
    struct Layer {
        public: 

        /**
         * @brief Getting the layer mapping.
         * I couldn't find any better way to handle it yet. 
        */
        void Set(const pugi::xml_node_iterator& in) {
            mapcsv = (*in).first_child().value(); 
        }

        /**
         * @brief This has to be here for parsing. 
         * Fill function needs the tag to find the attributes and mapping of layer.
         * It has to be constant expression and static, since the Fill function it self
         * is constant expression. 
         */
        const static inline std::string tag = "layer"; 
        /**
         * @brief Attributes
         * .tmx file contains these attributes in the layer node. They are also required
         * for rendering.
         */
        int id, width, height; 
        std::string name; 
        /**
         * @brief Reflection
         * The below macro call creates the Reflection part of the process.
         */
        DefineStructMembers(Layer, id, width, height, name); 


        /**
         * @brief Return the 2d array of map data converted from csv.
         * 
         * @return std::vector<std::vector<std::string>> 
         */
        std::vector<std::vector<std::string>> map_data_2d_string() const {
            std::stringstream ss; 
            ss << mapcsv; 
            std::vector<std::vector<std::string>> ret;
            for(std::string line; getline(ss, line);) {
                if(line.empty()) 
                    continue;
                std::stringstream cs;
                cs << line;
                std::vector<std::string> row;   
                for(std::string gid; getline(cs, gid, ',');) {
                    row.push_back(gid); 
                }
                ret.push_back(row); 
            }
            return ret; 
        }

        /**
         * @brief Return the 2d array of map data converted from csv
         * 
         * @return std::vector<std::vector<int>> 
         */
        std::vector<std::vector<int>> map_data_2d() const {
            std::stringstream ss; 
            ss << mapcsv; 
            std::vector<std::vector<int>> ret;
            for(std::string line; getline(ss, line);) {
                if(line.empty()) 
                    continue;
                std::stringstream cs;
                cs << line;
                std::vector<int> row;   
                for(std::string gid; getline(cs, gid, ',');) {
                    row.push_back(std::stoi(gid)); 
                }
                ret.push_back(row); 
            }
            return ret; 
        }

        /**
         * @brief Return the 1d array of map data converted from csv
         * Try using this vector with the indexes (x + (y * width))
         * @return std::vector<int> 
         */
        std::vector<int> map_data() const {
            std::stringstream ss; 
            ss << mapcsv; 
            std::vector<int> ret; 
            for(std::string line; getline(ss, line);) {
                if(line.empty())
                    continue; 
                std::stringstream cs; 
                cs << line; 
                for(std::string gid; getline(cs, gid, ',');) {
                    ret.push_back(std::stoi(gid)); 
                }
            }
            return ret; 
        }

        std::vector<GridTile> data_to_grid() {
            std::vector<GridTile> ret; 
            const auto& data = map_data();
            ret.reserve(width * height); 
            for(int y{}; y < height; y++) {
                for(int x{}; x < width; x++) {
                    auto index = x + y * width;
                    bool passable = data[index] == 0;  
                    ret.emplace_back(Geometry::Point{x, y}, passable); 
                }
            }
            return ret; 
        }
        
        Geometry::Size Size() {
            return {width, height};
        }

        bool is_passability_layer() {
            return String::ToLower(name).find("passability") != -1;
        }

        private:
        std::string mapcsv; 

    }; 

    /**
     * @brief Map structure. 
     * 
     */
    struct Map {
        public: 
        /**
         * @brief Attributes
         * .tmx file contains this attributes for map node. 
         */
        int width, height, tilewidth, tileheight; 
        DefineStructMembers(Map, width, height, tilewidth, tileheight); 
        static const inline std::string tag = "map"; 
        
        Map(const std::string& file_name, size_t tileset_count, size_t layer_count, size_t objectgroup_count) : TileSets(tileset_count), Layers(layer_count), ObjectGroups(objectgroup_count) {
            pugi::xml_document doc; 
            if(!doc.load_file(file_name.c_str())) {
                throw Exception::ParseFailed(); 
            }
            doc.child("map").append_child("dummy");
            /*
             * Parameter initializiation. 
             * first node and last node should be set. If you directly give
             * *.begin() or *.end(), it confuses with other overloads, 
             * and it gives a compile error. 
             */
            pugi::xml_node_iterator first_node = doc.child("map").begin(); 
            pugi::xml_node_iterator last_node = doc.child("map").end();
            /*
             * Fill function checks in a range. If so we need to lower the end once
             * to prevent it from collapsing. 
             */
            last_node--;
            using namespace Parse::Filler;
            /*
             * Call fill for TileSets
             */
            Fill(TileSets, first_node, last_node); 
            /*
             * Call fill for Layers
             */
            Fill(Layers, first_node, last_node); 

            Fill(ObjectGroups, first_node, last_node); 


            /*
             *  Dummy array for passing this to fill
             */
            std::array<Map*,1> this_ {this}; 
            /*
             * Reset the first and last node to doc first node
             */
            first_node = doc.begin(); 
            last_node = doc.end();
            /*
             * Call the Fill function again.
             */
            Fill<0, 1, Map>(this_, first_node, last_node);  
        }
        
        /**
         * @brief Constructor
         * Requires the file_name to parse the file. 
         * @param file_name 
         */
        Map(const std::string& file_name, size_t tileset_count, size_t layer_count) : TileSets(tileset_count), Layers(layer_count) {
            pugi::xml_document doc; 
            if(!doc.load_file(file_name.c_str())) {
                throw Exception::ParseFailed(); 
            }
            doc.child("map").append_child("dummy");
            /*
             * Parameter initializiation. 
             * first node and last node should be set. If you directly give
             * *.begin() or *.end(), it confuses with other overloads, 
             * and it gives a compile error. 
             */
            pugi::xml_node_iterator first_node = doc.child("map").begin(); 
            pugi::xml_node_iterator last_node = doc.child("map").end();
            /*
             * Fill function checks in a range. If so we need to lower the end once
             * to prevent it from collapsing. 
             */
            last_node--;
            using namespace Parse::Filler;
            /*
             * Call fill for TileSets
             */
            Fill(TileSets, first_node, last_node); 
            /*
             * Call fill for Layers
             */
            Fill(Layers, first_node, last_node); 

            /*
             *  Dummy array for passing this to fill
             */
            std::array<Map*,1> this_ {this}; 
            /*
             * Reset the first and last node to doc first node
             */
            first_node = doc.begin(); 
            last_node = doc.end();
            /*
             * Call the Fill function again.
             */
            Fill<0, 1, Map>(this_, first_node, last_node);  
        }


        Map(const std::string& file_name) {
            pugi::xml_document doc; 
            if(!doc.load_file(file_name.c_str())) {
                throw Exception::ParseFailed(); 
            }
            doc.child("map").append_child("dummy"); 

            auto TileSetNodes = doc.child("map").children(TileSet::tag.c_str()); 
            auto LayerNodes = doc.child("map").children(Layer::tag.c_str()); 
            auto OgNodes = doc.child("map").children(ObjectGroup::tag.c_str()); 

            using namespace Parse::Filler; 

            Fill(TileSets, TileSetNodes); 
            Fill(Layers, LayerNodes); 
            Fill(ObjectGroups, OgNodes); 

            /*
             *  Dummy array for passing this to fill
            */
            std::array<Map*,1> this_ {this}; 
            /*
             * Reset the first and last node to doc first node
             */
            auto first_node = doc.begin(); 
            auto last_node = doc.end();
            /*
             * Call the Fill function again.
             */
            Fill<0, 1, Map>(this_, first_node, last_node);  

        }
        /**
         * @brief Dummy constructor.
         * Don't use it!
         */
        Map() {}

        /**
         * @brief Get the Tile Set object (reference)
         * 
         * @tparam Index 
         * @return const struct TileSet& 
         */
        template<size_t Index> const struct TileSet& GetTileSet() { return TileSets[Index]; }
        /**
         * @brief Get the Tile Set object (const)
         * 
         * @tparam Index 
         * @return const struct TileSet 
         */
        template<size_t Index> const struct TileSet GetTileSet() const{ return TileSets[Index]; }
        /**
         * @brief Get the Tile Set object (reference)
         * 
         * @param Index 
         * @return struct TileSet& 
         */
        struct TileSet& GetTileSet(size_t Index) { return TileSets[Index]; }
        /**
         * @brief Get the Tile Set object (const)
         * 
         * @param Index 
         * @return struct TileSet 
         */
        struct TileSet GetTileSet(size_t Index) const { return TileSets[Index]; }
        /**
         * @brief Get the Tile Set array (reference)
         * 
         * @return std::array<TileSet, 2>& 
         */
        std::vector<TileSet>& GetTileSets() { return TileSets; }
        /**
         * @brief Get the Tile Sets object (const)
         * 
         * @return std::array<TileSet, 2> 
         */
        std::vector<TileSet> GetTileSets() const { return TileSets; }
        /**
         * @brief Get the Layer object (reference)
         * 
         * @tparam Index 
         * @return const struct Layer& 
         */
        template<size_t Index> const struct Layer& GetLayer() { return Layers[Index]; }
        /**
         * @brief Get the Layer object (const)
         * 
         * @tparam Index 
         * @return const struct Layer 
         */
        template<size_t Index> const struct Layer GetLayer() const { return Layers[Index]; }
        /**
         * @brief Get the Layer object (reference)
         * 
         * @param Index 
         * @return struct Layer& 
         */
        struct Layer& GetLayer(int Index) { return Layers[Index]; }
        /**
         * @brief Get the Layer object (const)
         * 
         * @param Index 
         * @return struct Layer 
         */
        struct Layer GetLayer(int Index) const { return Layers[Index]; }
        /**
         * @brief Get the Layers object (reference)
         * 
         * @return std::array<Layer, 2>& 
         */
        std::vector<Layer>& GetLayers() { return Layers; }
        /**
         * @brief Get the Layers object (const)
         * 
         * @return std::array<Layer, 2> 
         */
        std::vector<Layer> GetLayers() const { return Layers; }

        ObjectGroup& GetObjectGroup(size_t index) {
            return ObjectGroups[index]; 
        }

        ObjectGroup GetObjectGroup(size_t index) const {
            return ObjectGroups[index]; 
        }
        
        std::unordered_map<int, ObjectGroup> GetObjectGroupMap() const {
            if (ObjectGroups.empty()) {
                return {}; 
            }
            std::unordered_map<int, ObjectGroup> ret; 
            for(auto group : ObjectGroups) {
                ret[group.id] = group; 
            }
            return ret; 
        } 

        std::vector<ObjectGroup>& GetObjectGroups() {
            return ObjectGroups; 
        }

        std::vector<ObjectGroup> GetObjectGroups() const {
            return ObjectGroups; 
        }

        std::unordered_map<int, int> ObjectLayerOrder() const {
            std::unordered_map<int, int> ret; 
            for(auto group : ObjectGroups) {
                ret[group.previous_layer_index] = group.id; 
            }
            return ret; 
        }

        /**
         * @brief Get the Passability Layer 
         * 
         * @return Returns empty layer if not found.
         */
        Layer& GetPassabilityLayer() {
            for(auto& layer : GetLayers()) {
                if(layer.is_passability_layer()) {
                    return layer; 
                }
            }
            return *new Layer{};
        }


        private: 
        std::vector<TileSet> TileSets;
        std::vector<Layer> Layers;  
        std::vector<ObjectGroup> ObjectGroups; 
    }; 
}