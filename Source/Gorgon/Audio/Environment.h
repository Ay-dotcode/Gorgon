#pragma once

#include "../Geometry/Point3D.h"


namespace Gorgon { namespace Audio {
    
    class Environment;
    
    /**
     * Listener class sets the properties for the audio listener. It is used for positional
     * audio. If enabled, listener position will cause audio lag and the speed will be used
     * to calculate doppler shift. The environment of the listener should not be destroyed
     * before listener. There are no checks performed.
     */
    class Listener {
        friend void AudioLoop();
    public:
        
        Listener(Environment &env) : env(&env) { }
        
        /// Changes the current location of the listener, if automatic speed calculation is
        /// set, this function will infer speed and the location of the listener.
        void SetLocation(const Geometry::Point3D &location) {
            this->location = location;
        }

		void SetUpDirection(const Geometry::Point3D &up, const Geometry::Point3D &orientation) {
			this->up = up;
			SetOrientation(orientation);
		}

		/// Changes the orientation of the listener. This function is not cheap and should not be
		/// used if it can be avoided. In the default orientation Z is up and the listener is facing +
		/// in the Y axis
		void SetOrientation(const Geometry::Point3D &location) {
			this->orientation = orientation;
			earfactor = orientation.CrossProduct(up);
		}
        
        /// Returns the current location of the listener.
        Geometry::Point3D GetLocation() const {
            return location;
        }
        
        Geometry::Point3D LeftEar() const;
        
        Geometry::Point3D RightEar() const;

    private:
        Geometry::Point3D location    = {0, 0, 0};
		Geometry::Point3D orientation = {0, 1, 0};
		Geometry::Point3D up		  = {0, 0, 1};
		Geometry::Point3D earfactor   = {1, 0, 0};

        Environment *env;
    };

    
    /**
     * The environment which the audio system works on. To change units, you should set speed of sound
     * in units per second. Default unit is meters. All other metrics work relative to this value.
     * Max distance is set to 1 sound second, you may want to set this value as it might be short
     * for strategy games, or too far for rpg games.
     */
    class Environment {
        friend void AudioLoop();
        friend class Listener;
    public:
        Environment() : listener(*this) {
            init();
        }
        
        /// Currently active environment.
        static Environment Current;
        
    private:
        void init();
        
        float unitspermeter      = 1;
        
        float speedofsound       = 340.29f; //units/sec
        
        float recordingdistance  = 1.f;     //units
        
        float attuniationfactor  = 0.1f;    //Higher values will attenuation more
        
        float maxdistance        = 200.f;   //units
        
        float nonblocked         = 0.2f;    //amount of sound that is not blocked by head
        
        float headradius         = 0.15f;    //units, rough size of the head
        
        float hrtfdistance       = 0.666f;   //units, rough perimeter that the sound should travel to reach other ear ²  
        
        Geometry::Point3D left, right;     //vectors for left and right ear hearing ²
        
        Geometry::Point3D speaker_locations[4] = {
            { .1f,-.1f, 0.f},
            {-.1f,-.1f, 0.f},
            { .1f, .3f, 0.f},
            {-.1f, .3f, 0.f},
        };
        
        Geometry::Point3D speaker_vectors[4];
        float              speaker_boost[4];
        
        // ² calculated
        
        /// Currently active listener.
        Listener listener;
    };
    
    inline Geometry::Point3D Gorgon::Audio::Listener::LeftEar() const {
        return location - earfactor;
    }
    
    inline Geometry::Point3D Gorgon::Audio::Listener::RightEar() const {
        return location + earfactor;
    }

} }
