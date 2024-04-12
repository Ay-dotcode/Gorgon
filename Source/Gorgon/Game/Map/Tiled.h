#pragma once 
#include <Gorgon/Game/Parse/FillerSystem/Filler.h>
#include <array>
#include <sstream>
#include <string>
#include <Gorgon/Struct.h>
#include <vector>


namespace Gorgon::Game::Map::Tiled {
    struct TiledMap {
    private: 
    struct TileSet {
        public: 
        /* MUST BE ARRAY AND MUST BE CONST CHAR *  */
        constexpr static inline std::string tag = "tileset";  
        std::string firstgid = "0"; 
        int tilewidth, tileheight;
        std::string name;  
        DefineStructMembers(TileSet, firstgid,  tilewidth, tileheight, name);  
        pugi::xml_node inner; 
        void SetInner(pugi::xml_node inner) {
            this->inner = inner;
        }

    }; 
    
    struct Layer {
        private: 
        public: 
        constexpr static inline std::string tag = "layer"; 

        int id, width, height; 
        std::string name; 
        DefineStructMembers(Layer, id, width, height, name);  
        pugi::xml_node inner; 

        std::string layer_map$str; 
        Layer() {
            layer_map$str = inner.first_child().value();
        }

        template<size_t N_>
        auto MapLocationsWithTileSet(const std::array<TileSet, N_> tilesets) {
            std::stringstream ss;
            ss << layer_map$str;
            std::string line; 
            std::vector<std::vector<TileSet>> map;

            while(std::getline(ss, line)) {
                std::stringstream sts; 
                std::vector<TileSet> row;
                sts<< line; 

                if(line.empty()) {
                    continue;
                }

                std::string gid;

                while(std::getline(sts, gid, ',')) {
                    bool match = false; 
                    for(auto tileset : tilesets) {
                        if(tileset.firstgid == gid) {
                            row.push_back(tileset); 
                            match = true; 
                        }
                    }
                    if(not match) {
                        row.push_back(TileSet{"0"}); 
                    }
                }
                map.push_back(row);
            }

            return map; 
        }

        void SetInner(pugi::xml_node inner) {
            this->inner = inner;
            layer_map$str = inner.first_child().value();
        }
    };

    std::array<TileSet, 6> Tileset; 
    std::array<Layer, 5> LayerInfo; 
    std::array<std::vector<std::vector<TileSet>>, 5> LayerMapData; 

    public: 
    TiledMap(const std::string& file_name, const std::string& first_node = "map") {
        Game::Parse::Filler::Fill<0, 5, 4>(LayerInfo, file_name, first_node); 
        Game::Parse::Filler::Fill<0, 6, 4>(Tileset, file_name, first_node); 
        int i = 0; 
        for(auto layer: LayerInfo) {
            LayerMapData[i++] = layer.MapLocationsWithTileSet(Tileset);
        }
    } 

    template<size_t Index> 
    constexpr struct TileSet& GetTileset() {
        return Tileset[Index];
    }
    struct TileSet& GetTileset(int index) {
        return Tileset[index]; 
    }
    std::array<TileSet, 6>& GetTilesets() {
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
};

};