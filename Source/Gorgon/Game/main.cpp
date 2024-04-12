#include "PugiXML/pugixml.hpp"
#include <array>
#include <string>
#include <iostream>
#include <Gorgon/Struct.h>
#include "FillerSystem/Filler.h"


struct tileset {
    public: 
    /* MUST BE ARRAY AND MUST BE CONST CHAR *  */
    constexpr static inline std::string tag = "tileset";  

    int firstgid, tilewidth, tileheight;
    std::string name;  
    DefineStructMembers(tileset, firstgid,  tilewidth, tileheight, name);  
    pugi::xml_node inner; 

}; 


int main() {

    std::array<tileset, 6> testarr; 
    Filler::Fill<0, 6, 4>(testarr, "map.tmx", "map"); 
    
    for(auto x : testarr) {
        std::cout << x.name << std::endl; 
    } 

    return 0; 
}