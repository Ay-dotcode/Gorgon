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
        void SetLocation(const Geometry::Point3Df &location) {
            this->location = location;
        }
        
        /// Returns the current location of the listener.
        Geometry::Point3Df GetLocation() const {
            return location;
        }
        
        Geometry::Point3Df LeftEar() const;
        
        Geometry::Point3Df RightEar() const;

    private:
        Geometry::Point3Df location = {0, 0, 0};
        
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
        void init() {
            left  = { std::cos(   auricleangle), -std::sin(   auricleangle), 0};
            right = { std::cos(PI-auricleangle), -std::sin(PI-auricleangle), 0};
        }
        
        float unitspermeter      = 1;
        
        float speedofsound       = 340.29f; //units/sec
        
        float recordingdistance  = 1.f;     //units
        
        float attuniationfactor  = 0.5f;    //Higher values will attenuation more
        
        float maxdistance        = 200.f;   //units
        
        float nonblocked         = 0.2f;    //amount of sound that is not blocked by head
        
        float headradius         = 0.15f;    //units, rough size of the head
        
        float hrtfdistance       = 0.666f;   //units, rough perimeter that the sound should travel to reach other ear ²  
        
        float auricleangle       = 0.175f;   //this angle in radians covers the angle difference caused by auircle.
        
        Geometry::Point3Df left, right;     //vectors for left and right ear hearing ²
        
        Geometry::Point3Df speaker_locations[4] = {
            {-.33f, -.5f, 0.f},
            { .33f, -.5f, 0.f},
            {-.33f, 1.f , 0.f},
            { .33f, 1.f , 0.f},
        };
        
        Geometry::Point3Df speaker_vectors[4];
        
        // ² calculated
        
        /// Currently active listener.
        Listener listener;
    };
    
    Geometry::Point3Df Gorgon::Audio::Listener::LeftEar() const {
        return location - Geometry::Point3Df(env->headradius, 0, 0);
    }
    
    Geometry::Point3Df Gorgon::Audio::Listener::RightEar() const {
        return location + Geometry::Point3Df(env->headradius, 0, 0);
    }

} }
