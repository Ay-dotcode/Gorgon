#include "Environment.h"

 
namespace Gorgon { namespace Audio {
    void Environment::init() {
        left  = { std::cos(PI-auricleangle), -std::sin(PI-auricleangle), 0};
        right = { std::cos(   auricleangle), -std::sin(   auricleangle), 0};
        
        for(int i=0; i<4; i++) {
            speaker_vectors[i] = speaker_locations[i].Normalize() * -1;
            speaker_boost  [i] = speaker_locations[i].Distance();
        }
    }

} }
