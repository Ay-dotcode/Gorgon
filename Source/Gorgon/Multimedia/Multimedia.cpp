#include "../Multimedia.h"
#include "Stream.h"

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
        streamthread.join();
    }
    
}
    
    
    void StreamThread() {
        atexit(&internal::exitfn);
        
        while(!exiting) {
            for(auto &stream : Stream::streams) {
                stream.FillBuffer();
            }
            
            std::this_thread::sleep_for(1ms);
        }
    }
    
    void Initialize() {
        internal::streamthread = std::thread(&StreamThread);
    }
    
    Containers::Collection<Stream> Stream::streams;
    
} }
