#include "AudioStream.h"

#include "../Resource/Sound.h"


namespace Gorgon { namespace Multimedia {
    
    namespace internal {
        
        class WaveStreamStreamer : public AudioStreamer {
        public:
            /// Creates a wave stream streamer. Stream location should be at the start of the data.
            WaveStreamStreamer(std::istream &stream, bool ownstream, AudioStream &audio) :
                stream(stream), ownstream(ownstream), audio(audio)
            { }
            
            ~WaveStreamStreamer() {
                if(ownstream)
                    delete &stream;
            }
            
            unsigned long Init(Containers::Wave &target) override {
                unsigned long size;
                
                target.ImportWav(stream, false, size);
                
                return size;
            }
            
            virtual void LoadData() override {
                
            }
            
        private:
            std::istream &stream;
            bool ownstream;
            AudioStream &audio;
        };
    }

    bool AudioStream::Stream(const std::string &filename) {
        auto dotpos = filename.find_last_of('.');

        if(dotpos != -1) {
            auto ext = filename.substr(dotpos+1);

            if(String::ToLower(ext) == "wav") {
                return StreamWav(filename);
            }
/*#ifdef FLAC_SUPPORT
            else if(String::ToLower(ext) == "flac") {
                return StreamFLAC(filename);
            }
#endif*/
        }
        
        std::ifstream file(filename, std::ios::binary);
        

        if(file.is_open())
            return Stream(file);
        else
            return false;
    }
    
    bool AudioStream::Stream(std::istream &file) {
        static const uint32_t wavsig  = 0x46464952;
        static const uint32_t flacsig = 0x43614c66;

        uint32_t sig = IO::ReadUInt32(file);
        file.seekg(0, std::ios::beg);

        if(sig == wavsig) {
            return StreamWav(file);
        }
/*#ifdef FLAC_SUPPORT
        else if(sig == flacsig) {
            return ImportFLAC(file);
        }
#endif*/

        throw std::runtime_error("Unsupported file format");
    }


} }
    
    
