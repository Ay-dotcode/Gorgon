#pragma once

#include "../Containers/Wave.h"
#include "../Containers/Collection.h"
#include "../Audio/Source.h"
#include "../Utils/Assert.h"

#include <thread>

namespace Gorgon { 
    
namespace Resource {
    class Sound;
}

namespace Multimedia {
    
    /// @cond internal
    namespace internal {
        
        /// This class is the interface for all audiostreamers
        class AudioStreamer {
        public:
            
            /// This functions loads all necessary information to wave container without actually
            /// loading any data. Returns the total number of samples in the stream source.
            virtual unsigned long Init(Containers::Wave &target) = 0;
            
            /// This function should check if there is data to be loaded. If there is, streamer
            /// should decode and pass data to audiostream. If there is any extra data that is 
            /// decoded, it should stay with the streamer.
            virtual void LoadData() = 0;
        };
        
        /// Currently active streamers
        extern Containers::Collection<AudioStreamer> streamers;
        
        /// Performs stream loading
        extern std::thread streamthread;
    }
    /// @endcond
    

    /**
     * Allows streaming audio from various sources. You should only create a single controller from
     * an audio stream. Multiple controllers will cause misses and since there is no seeking between
     * controllers, the missing audio samples will be replaced with silence. Streams are extracted
     * in a different thread, leaving audio thread free to perform audio operations. Multiple 
     * buffers are used to ensure buffer underrun does not happen and audio will continue even if
     * a seek operation is requested. When seek is requested, a new buffer is loaded and once ready,
     * seek operation will actually be performed. Audio will continue to play during seek operation
     * from the old location.
     */
    class AudioStream : public Audio::Source {
    public:
        
        AudioStream(unsigned long buffersize = 32*1024);
        
        /// Starts seeking the stream to the given point. Only one buffer will start loading. The 
        /// other two buffers will continue playing from the old point. Once the data is started
        /// streaming from this new location, other buffers will be loaded as well.
        
        
        /// Returns if the streaming system is ready to seek to given location.
        
        
        /// Starts streaming the given file. File type will be determined automatically from the 
        /// extension. Only a portion of the file will be loaded immediately and it will be loaded
        /// automatically as necessary. Returns false if the file cannot be read.
        ///
        /// @warning  Streaming works if this object is only used by a single
        /// controller. Multiple controllers will cause stream to switch back and forth causing 
        /// issues.
        bool Stream(const std::string &filename);
        
        /// Starts streaming the given file. File type will be determined automatically from the 
        /// extension. Only a portion of the file will be loaded immediately and it will be loaded
        /// automatically as necessary. Returns false if the file cannot be read.
        ///
        /// @warning  Streaming works if this object is only used by a single
        /// controller. Multiple controllers will cause stream to switch back and forth causing 
        /// issues.
        bool Stream(std::istream &stream);
        
        /// Starts streaming the given resource. Only a portion of the resource will be loaded 
        /// immediately and it will be loaded automatically as necessary. Returns false if the 
        /// resource cannot be read.
        ///
        /// @warning  Streaming works if this object is only used by a single
        /// controller. Multiple controllers will cause stream to switch back and forth causing 
        /// issues.
        bool Stream(Resource::Sound &source);
        
        /// Starts streaming the given wav file. Only a portion of the file will be loaded 
        /// immediately and it will be loaded automatically as necessary. Returns false if the file
        /// cannot be read.
        ///
        /// @warning  Streaming works if this object is only used by a single
        /// controller. Multiple controllers will cause stream to switch back and forth causing 
        /// issues.
        bool StreamWav(const std::string &filename);
        
        /// Starts streaming the given wav file. Only a portion of the file will be loaded 
        /// immediately and it will be loaded automatically as necessary. Returns false if the file
        /// cannot be read.
        ///
        /// @warning  Streaming works if this object is only used by a single
        /// controller. Multiple controllers will cause stream to switch back and forth causing 
        /// issues.
        bool StreamWav(std::istream &stream);
        
#ifdef FLAC_SUPPORT
        /// Starts streaming the given FLAC file. Only a portion of the file will be loaded 
        /// immediately and it will be loaded automatically as necessary. Returns false if the file
        /// cannot be read.
        ///
        /// @warning  Streaming works if this object is only used by a single
        /// controller. Multiple controllers will cause stream to switch back and forth causing 
        /// issues.
        bool StreamFLAC(const std::string &filename);
        
        /// Starts streaming the given FLAC file. Only a portion of the file will be loaded 
        /// immediately and it will be loaded automatically as necessary. Returns false if the file
        /// cannot be read.
        ///
        /// @warning  Streaming works if this object is only used by a single
        /// controller. Multiple controllers will cause stream to switch back and forth causing 
        /// issues.
        bool StreamFLAC(std::istream &stream);
#endif
    
    private:
        class bufferdata {
            Containers::Wave buffer;
            unsigned long beg = 0;
            unsigned long end = 0; //+1 last item
        };
        
        /// Only the first buffer will contain valid information
        std::array<bufferdata, 3> buffers;
        
    };
    
} }
