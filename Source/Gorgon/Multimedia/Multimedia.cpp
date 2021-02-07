#include "../Multimedia.h"
#include "Stream.h"
#include "../Audio.h"

#include <cstdlib>

#include <thread>
#include <chrono>

using namespace std::chrono_literals;

namespace Gorgon { 
    extern bool exiting;
    
namespace Multimedia {

namespace internal {
    
    std::thread streamthread;
    
    void exitfn() {
        exiting = true;
        streamthread.join();
    }
    
}
    
    
    void StreamThread() {
        atexit(&internal::exitfn);
        
        while(!exiting) {
            for(auto &stream : Stream::streams) {
                try {
                    stream.FillBuffer();
                }
                catch(const std::exception &ex) {
                    Audio::Log << ex.what();
                }
            }
            
            std::this_thread::sleep_for(1ms);
        }
    }
    
    void Initialize() {
        internal::streamthread = std::thread(&StreamThread);
    }
    
    Containers::Collection<Stream> Stream::streams;
    
} }
