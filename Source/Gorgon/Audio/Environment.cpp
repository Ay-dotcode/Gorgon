#include "Environment.h"

 
namespace Gorgon { namespace Audio {
    void Environment::init() {
        left  = { std::cos(PI-auricleangle), -std::sin(PI-auricleangle), 0};
        right = { std::cos(   auricleangle), -std::sin(   auricleangle), 0};
            
        float maxboost = 0;
        for(int i=0; i<4; i++) {
            speaker_vectors[i] = listener.transform * (speaker_locations[i].Normalize() * -1);
            speaker_boost  [i] = speaker_locations[i].Distance();
			if(speaker_boost[i] > maxboost) maxboost = speaker_boost[i];
        }

		for(int i=0; i<4; i++) {
			speaker_boost[i] -= maxboost;
		}
    }
    
    void Environment::updateorientation() {
        for(int i=0; i<4; i++) {
            speaker_vectors[i] = listener.transform * (speaker_locations[i].Normalize() * -1);
        }
    }
    
    void Listener::orientcalc() {
        auto side = orientation.CrossProduct(up);
        transform(0, 0) = side.X;
        transform(1, 0) = side.Y;
        transform(2, 0) = side.Z;
        
        transform(0, 1) = orientation.X;
        transform(1, 1) = orientation.Y;
        transform(2, 1) = orientation.Z;
        
        transform(0, 2) = up.X;
        transform(1, 2) = up.Y;
        transform(2, 2) = up.Z;
        
        invtransform = transform.Transpose();
        
        poscalc();
        
        env->updateorientation();
    }

} }
