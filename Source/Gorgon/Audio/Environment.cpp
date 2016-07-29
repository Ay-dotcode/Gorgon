#include "Environment.h"

 
namespace Gorgon { namespace Audio {
    void Environment::init() {		float maxboost = 0;
        for(int i=0; i<4; i++) {
            speaker_vectors[i] = speaker_locations[i].Normalize() * -1;
            speaker_boost  [i] = speaker_locations[i].Distance();
			if(speaker_boost[i] > maxboost) maxboost = speaker_boost[i];
        }

		for(int i=0; i<4; i++) {
			speaker_boost[i] -= maxboost;
		}
    }

} }
