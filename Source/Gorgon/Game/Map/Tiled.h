#pragma once 
#ifndef TILED_H
#define TILED_H
#include <Gorgon/Game/Parse/FillerSystem/Filler.h>
#include <array>
#include <sstream>
#include <string>
#include <Gorgon/Struct.h>
#include <vector>


namespace Gorgon::Game::Map::Tiled {
    // The example tile map class, which is used to parse the Tiled map files.
    // This class can also be used for rendering purposes. The generic renderer will 
    // be based on this example class. If you configure your class the same way, you can
    // use the generic renderer.

    // Tiled TileSet. 
    struct TileSet {
        public: 
        // Filler system requires the tag to be set so it can search for it in the XML file.
        // It must be constexpr static inline.
        constexpr static inline std::string tag = "tileset";  

        // Tilesets has this attributes in the .TMX file.
        // Note: .TMX file is the XML file format that Tiled uses.
        std::string firstgid = "0"; 
        int tilewidth, tileheight;
        std::string name;  
        
        // This is a macro that is used to define the members of the struct.
        // Use it with order of your members.
        // If you mess the orders below, it'll have the names as you give but
        // it'll try to access wrong member. So use it in order.
        DefineStructMembers(TileSet, firstgid,  tilewidth, tileheight, name);  
        
        // This is the inner node of the XML file.
        // I didn't come up with an idea to parse this yet. 
        // Since in an XML file you can have multiple inner nodes or
        // all the same tags can have these inner nodes. 
        // Checking for the next one is a challenge. 
        // So I left at it is. 
        // Pugi supports next_sibling() function. And you can use that for 
        // getting the next sibling. 
        pugi::xml_node inner; 
        void SetInner(const pugi::xml_node& inner) {
            this->inner = inner;
        }

    }; 
    // Tiled Layer.
    struct Layer {
        private: 
        public: 
        // Filler system requires the tag to be set so it can search for it in the XML file.
        // It must be constexpr static inline.
        constexpr static inline std::string tag = "layer"; 

        // Layer has this attributes in the .TMX file.
        // Note: .TMX file is the XML file format that Tiled uses.
        int id, width, height; 
        std::string name; 

        // This is a macro that is used to define the members of the struct.
        // Use it with order of your members.
        // If you mess the orders below, it'll have the names as you give but
        // it'll try to access wrong member. So use it in order.
        DefineStructMembers(Layer, id, width, height, name);  
        
        // This is the inner node of the XML file.
        // I didn't come up with an idea to parse this yet.
        // Since in an XML file you can have multiple inner nodes or
        // all the same tags can have these inner nodes.
        // Checking for the next one is a challenge.
        // So I left at it is.
        // Pugi supports next_sibling() function. And you can use that for
        // getting the next sibling.
        pugi::xml_node inner; 

        // This is the map of the layer.
        // It's a string because it's a CSV file.
        std::string layer_map$str; 


        Layer() {
            layer_map$str = inner.first_child().value();
        }

        // Function maps the locations with the tileset and returns a 2D vector.
        template<size_t N_>
        auto MapLocationsWithTileSet(const std::array<TileSet, N_>& tilesets) {
            // It's not the most memory efficient way to do this.
            // But it's easier to implement.
            std::stringstream ss;
            ss << layer_map$str;
            std::string line; 
            std::vector<std::vector<TileSet>> map;

            // CSV system breaks the items by new line for each row.
            while(std::getline(ss, line)) {
                std::stringstream sts; 
                std::vector<TileSet> row;
                sts<< line; 
                // If line is empty, we don't need to check for anything, so continue. 
                if(line.empty()) {
                    continue;
                }
                std::string gid;

                // CSV system breaks the items by comma for each column.
                while(std::getline(sts, gid, ',')) {
                    // if matches, than push back the tileset.
                    bool match = false; 
                    for(auto tileset : tilesets) {
                        if(tileset.firstgid == gid) {
                            row.push_back(tileset); 
                            match = true; 
                        }
                    }
                    // no match, push back an empty tileset.
                    // This is required because the map must be filled with the same amount of tilesets.
                    if(not match) {
                        row.push_back(TileSet{"0"}); 
                    }
                }
                // Push back the row to the map.
                map.push_back(row);
            }
            return map; 
        }
        auto MapLocationsWithTileSet(const std::vector<TileSet>& tilesets) {
            // It's not the most memory efficient way to do this.
            // But it's easier to implement.
            std::stringstream ss;
            ss << layer_map$str;
            std::string line; 
            std::vector<std::vector<TileSet>> map;

            // CSV system breaks the items by new line for each row.
            while(std::getline(ss, line)) {
                std::stringstream sts; 
                std::vector<TileSet> row;
                sts<< line; 
                // If line is empty, we don't need to check for anything, so continue. 
                if(line.empty()) {
                    continue;
                }
                std::string gid;

                // CSV system breaks the items by comma for each column.
                while(std::getline(sts, gid, ',')) {
                    // if matches, than push back the tileset.
                    bool match = false; 
                    for(auto tileset : tilesets) {
                        if(tileset.firstgid == gid) {
                            row.push_back(tileset); 
                            match = true; 
                        }
                    }
                    // no match, push back an empty tileset.
                    // This is required because the map must be filled with the same amount of tilesets.
                    if(not match) {
                        row.push_back(TileSet{"0"}); 
                    }
                }
                // Push back the row to the map.
                map.push_back(row);
            }
            return map; 
        }
        void SetInner(pugi::xml_node inner) {
            this->inner = inner;
            layer_map$str = inner.first_child().value();
        }
    };
    
    struct TiledMap {
    public:
    static constexpr inline std::string tag = "map"; 
    int tilewidth, tileheight;
    DefineStructMembers(TiledMap, tilewidth, tileheight);  
    
    void SetInner(pugi::xml_node) {} 

    TiledMap(const std::string& file_name, const std::string& first_node = "map") : Tileset(6) {
        Game::Parse::Filler::Fill<0, 5>(LayerInfo, file_name, first_node); 
        Game::Parse::Filler::Fill(Tileset, file_name, first_node); 
        int i = 0; 
        for(auto layer: LayerInfo) {
            LayerMapData[i++] = layer.MapLocationsWithTileSet(Tileset);
        }
        std::array<TiledMap, 1> this_ {*this}; 
        Game::Parse::Filler::Fill<0, 1>(this_, file_name, ""); 
    } 

    // Getters; 
    template<size_t Index> 
    constexpr struct TileSet& GetTileset() {
        return Tileset[Index];
    }
    struct TileSet& GetTileset(int index) {
        return Tileset[index]; 
    }
    std::vector<TileSet>& GetTilesets() {
        return Tileset; 
    }
    template<size_t Index> 
    constexpr struct Layer& GetLayerInfo() {
        return LayerInfo[Index];
    }
    struct Layer& GetLayerInfo(int index) {
        return LayerInfo[index]; 
    }
    std::array<Layer, 5>& GetLayerInfo() {
        return LayerInfo; 
    }
    std::array<std::vector<std::vector<TileSet>>, 5>& GetLayerMapData() {
        return LayerMapData; 
    }
    size_t GetLayerCount() {
        return LayerInfo.size(); 
    }
    size_t GetTilesetCount() {
        return Tileset.size(); 
    }

    private: 
    // My Map has 6 tilesets and 5 layers.
    // We can both use the array or vector.
    // Array will be faster but vector is more flexible.
    // You can choose both. Remember to resize your vector at constructor.
    // Because the vector is empty at the beginning, so you need to resize it.
    // The fill system still requires the size of the items, because it doesn't 
    // know how many times the tag appears in the XML file. 
    // We kinda depend on the user to give the correct size.
    std::vector<TileSet> Tileset; 
    std::array<Layer, 5> LayerInfo; 
    std::array<std::vector<std::vector<TileSet>>, 5> LayerMapData; 
};


struct StandardTile {};
struct IsometricTile {};


};
#endif