#include "AudioStream.h"

#include "../Resource/Sound.h"
#include "../Audio.h"


namespace Gorgon { 
    
namespace Audio { namespace internal {
    extern std::mutex ControllerMtx;
} }
    
namespace Multimedia {
    
    namespace internal {
        
        class WaveStreamStreamer : public AudioStreamer {
        public:
            /// Creates a wave stream streamer. Stream location should be at the start of the data.
            WaveStreamStreamer(std::istream &stream, bool ownstream) :
                stream(stream), ownstream(ownstream)
            { }
            
            ~WaveStreamStreamer() {
                if(ownstream)
                    delete &stream;
            }
            
            unsigned long Init(Containers::Wave &target) override {
                unsigned long size;
                
                target.ImportWav(stream, false, size, samplesize, blocksize);
                startoffset = stream.tellg();
                channels = target.GetChannelCount();
                
                return size;
            }
            
            virtual void LoadData(unsigned long samplestart, Containers::Wave &target) override {
                stream.clear(stream.eofbit);
                stream.seekg(startoffset + samplestart * blocksize, std::ios::beg);
                
                for(auto sample : target) {
                    for(int c=0; c<channels; c++) {
                        if(stream.eof()) {
                            sample[c] = 0;
                        }
                        else {
                            if(samplesize == 8) {
                                sample[c] = (IO::ReadUInt8(stream) / 255.f) * 2.f - 1.f;
                            }
                            else {
                                sample[c] = IO::ReadInt16(stream) / 32767.f;
                            }
                        }
                    }
                }
            }
            
        private:
            std::istream &stream;
            bool ownstream;
            std::size_t startoffset;
            int samplesize, blocksize;
            int channels;
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
        
        auto &file = *new std::ifstream(filename, std::ios::binary);
        
        if(file.is_open())
            return Stream(file, true);
        else {
            delete &file;
            
            return false;
        }
    }
    
    bool AudioStream::Stream(std::istream &file, bool ownstream) {
        static const uint32_t wavsig  = 0x46464952;
        static const uint32_t flacsig = 0x43614c66;

        uint32_t sig = IO::ReadUInt32(file);
        file.seekg(0, std::ios::beg);

        if(sig == wavsig) {
            return StreamWav(file, ownstream);
        }
/*#ifdef FLAC_SUPPORT
        else if(sig == flacsig) {
            return ImportFLAC(file);
        }
#endif*/

        throw std::runtime_error("Unsupported file format");
    }
    
    bool AudioStream::StreamWav(const std::string &filename) {
        auto &file = *new std::ifstream(filename, std::ios::binary);
        
        if(file.is_open())
            return StreamWav(file, true);
        else {
            delete &file;
            return false;
        }
    }
    
    bool AudioStream::StreamWav(std::istream &file, bool ownstream) {
        delete streamer;
        
        try {
            streamer = new internal::WaveStreamStreamer(file, ownstream);
        }
        catch(...) {
            if(ownstream)
                delete &file;
            throw;
        }
        
        //clean up
        buffers[0].buffer.Destroy();
        
        ///load metadata 
        totalsize = streamer->Init(buffers[0].buffer);
        
        //overwrite it to other buffers 
        for(int i=1; i<buffers.size(); i++)
            buffers[i].buffer = buffers[0].buffer.Duplicate();
        
        if(totalsize == 0)
            return false;
        
        for(int i=0; i<buffers.size(); i++)
            buffers[i].buffer.Resize(buffersize);
        
        
        return true;
    }
    
    void AudioStream::FillBuffer() {
        if(!streamer)
            return;
        
        int lastsample = this->lastsample;//local copy
        
        if(isseeking && !seekcomplete) {
            
            //find the last sample in the buffers
            for(int i=0; i<buffers.size(); i++) {
                if(seektarget >= buffers[i].beg  && seektarget < buffers[i].end) {
                    seekcomplete = true;
                    return;
                }
            }
            
        
            int sampleind = -1;
            
            //find the last sample in the buffers
            for(int i=0; i<buffers.size(); i++) {
                if(lastsample >= buffers[i].beg  && lastsample < buffers[i].end)
                    sampleind = i;
            }
            
            int loadbuffer = 0;
            
            if(sampleind != -1) {
                sampleind = (sampleind + 2) % buffers.size();
            }
            
            
            auto &loading = buffers[loadbuffer];
            this->loadbuffer(loading, seektarget);
            return;
        }
        
        int sampleind = -1;
        int loadbuffer= -1;
        unsigned long startoff = lastsample;
        
        //find the last sample in the buffers
        for(int i=0; i<buffers.size(); i++) {
            if(lastsample >= buffers[i].beg  && lastsample < buffers[i].end)
                sampleind = i;
        }
        
        if(sampleind == -1) {
            loadbuffer = 0;
        }
        else {
            for(int i=1; i<buffers.size(); i++) {
                auto cur = (sampleind + i)%buffers.size();
                auto prev = (cur + buffers.size() - 1) % buffers.size();
                auto p = buffers[prev].end;
                if(p == totalsize)
                    p = 0;
                
                //if the buffer does not continue where the last is left off
                if(p != buffers[cur].beg) {
                    
                    //and not used for seeking
                    if(isseeking && seektarget >= buffers[cur].beg && buffers[cur].end)
                        continue;
                    
                    startoff = p;
                    loadbuffer = cur;
                    
                    break;
                }
            }
        }
        
        if(loadbuffer == -1)
            return;
        
        auto &loading = buffers[loadbuffer];
        
        if(isseeking && seektarget >= loading.beg && seektarget < loading.end)
            return;
        
        this->loadbuffer(loading, startoff);
        
        
        Audio::Log << "Stream buffer " << loadbuffer << " loaded from " << loading.beg << " to " << loading.end/* << " including: " << loading.buffer(0, 0) << ", " << loading.buffer(1, 0) << ", " << loading.buffer(2, 0)*/;
    }
    
    void AudioStream::loadbuffer(bufferdata &buffer, unsigned long startoff) {
        //TODO check filesize
        streamer->LoadData(startoff, buffer.buffer);
        
        
        std::lock_guard<std::mutex> g(Audio::internal::ControllerMtx);
        
        buffer.beg = startoff;
        buffer.end = std::min(totalsize, startoff + buffer.buffer.GetSize());
    }

    AudioStream::SeekResult AudioStream::StartSeeking(long unsigned int target) {
        std::lock_guard<std::mutex> g(Audio::internal::ControllerMtx);
        
        isseeking  = true;
        seektarget = target;

        return Pending;
    }

} }
