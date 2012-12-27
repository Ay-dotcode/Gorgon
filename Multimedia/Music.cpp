#include "Music.h"
#include "../Utils/UtilsBase.h"
#include <fstream>
#include <algorithm>
#include "../External/OpenAL/al.h"
#include "../External/vorbis/vorbisfile.h"


namespace gge { namespace multimedia {

	int StreamingMusic::DefaultBufferSize = 1024*128;
	int StreamingMusic::DefaultNumberOfBuffers = 3;
	std::vector<StreamingMusic*> StreamingMusic::members;

	int ogg_seek( void *vs, ogg_int64_t to, int type ) {
		VorbisStream *v = reinterpret_cast<VorbisStream*>(vs);

		if(v->stream) {
			v->stream->clear();
			v->stream->seekg(to, type);
		}

		return 0;
	}

	long ogg_tell( void *vs ) {
		VorbisStream *v = reinterpret_cast<VorbisStream*>(vs);

		if(v->stream)
			return (long)v->stream->tellg();
		else
			return 0;
	}

	size_t ogg_read(void* dest, size_t size1, size_t size2, void* vs) {
		VorbisStream *v=reinterpret_cast<VorbisStream*>(vs);


		if(v->stream) {
			v->stream->read((char*)dest,size1*size2);
			return (size_t)v->stream->gcount();
		}
		else
			return 0;
	}

	bool VorbisStream::Open(const std::string filename) {
		protect.Lock();

		if(cleanstream) utils::CheckAndDelete(stream);

		std::ifstream *file=new std::ifstream;

		file->open(filename, std::ios::binary);
		stream=file;

		cleanstream=true;

		if(file->is_open())
			setupread();

		protect.Unlock();

		return file->is_open();
	}

	void VorbisStream::Use(std::istream &stream) {
		protect.Lock();
		if(cleanstream) utils::CheckAndDelete(this->stream);

		this->stream=&stream;

		setupread();

		protect.Unlock();
	}

	void VorbisStream::Close() {
		if(this->stream && dynamic_cast<std::ifstream*>(this->stream)) {
			dynamic_cast<std::ifstream*>(this->stream)->close();
		}	
		ov_clear(ogg);
		info=NULL;
	}

	VorbisStream::VorbisStream(const std::string filename) {
		init();
		Open(filename);
	}

	VorbisStream::VorbisStream(std::ifstream &stream) {
		init();
		Use(stream);
	}

	VorbisStream::VorbisStream() {
		init();
	}

	void VorbisStream::init() {
		ogg=new OggVorbis_File;
		std::memset(ogg, 0, sizeof(OggVorbis_File));

		stream=NULL;
		info=NULL;
		cleanstream=false;
	}

	VorbisStream::~VorbisStream() {
		if(cleanstream) utils::CheckAndDelete(stream);

		ov_clear(ogg);
		delete ogg;
	}

	void VorbisStream::setupread() {
		if(controller==0)
			Music::initalize();
		if(buffers[0]==0)
			recreatebuffers();
		alSourceStop(controller);
		alSourceUnqueueBuffers(controller, buffers.size(), &buffers[0]);
		buffersattached=false;

		ov_clear(ogg);
		std::memset(ogg, 0, sizeof(OggVorbis_File));

		ov_callbacks callbacks;
		callbacks.read_func=&ogg_read;
		callbacks.seek_func=&ogg_seek;
		callbacks.tell_func=&ogg_tell;
		callbacks.close_func=NULL;
		int error = ov_open_callbacks((void*)this, ogg, NULL, -1, callbacks);
		
		info = ov_info(ogg, -1);
		if(!info) {
			Close();
			return;
		}

		freq=info->rate;
		format = (info->channels==1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

		dobuffercheck();
	}


	void StreamingMusic::CheckAll() {
		std::for_each(members.begin(), members.end(), [](StreamingMusic *member){
			member->buffercheck();
		});
	}

	StreamingMusic::StreamingMusic() {
		members.push_back(this);

		buffers.resize(DefaultNumberOfBuffers);
		alGenBuffers(buffers.size(), &buffers[0]);

		buffersize=DefaultBufferSize;
		buffersattached=false;
	}

	void StreamingMusic::recreatebuffers() {
		buffers.resize(buffers.size());
		alGenBuffers(buffers.size(), &buffers[0]);
	}

	StreamingMusic::~StreamingMusic() {
		for(auto it=members.begin();it!=members.end();++it) {
			if(*it==this) {
				members.erase(it);
				break;
			}
		}

		alDeleteBuffers(buffers.size(), &buffers[0]);
	}

	void StreamingMusic::dobuffercheck() {
		int processed=0;

		if(buffersattached) {
			alGetSourcei(controller, AL_BUFFERS_PROCESSED, &processed);
			for(int i=0;i<processed;i++) {
				sound::system::SoundBufferHandle buffer;

				alSourceUnqueueBuffers(controller, 1, &buffer);

				if(data.size()!=buffersize)
					data.resize(buffersize);
			
				int size=decode(data);
				alBufferData(buffer, format, &data[0], size, freq);

				alSourceQueueBuffers(controller, 1, &buffer);
			}
		}
		else {
			for(unsigned i=0;i<buffers.size();i++) {
				if(data.size()!=buffersize)
					data.resize(buffersize);

				int size=decode(data);
				alBufferData(buffers[i], format, &data[0], size, freq);
				int error=alGetError();

				alSourceQueueBuffers(controller, 1, &buffers[i]);
				buffersattached=true;
			}
		}
	}

	int VorbisStream::decode(std::vector<Byte> &buffer) {
		unsigned size=0;
		int section=0;

		while(size < buffer.size())
		{
			int result = ov_read(ogg, (char*)&data[size], buffer.size()-size, 0, 2, 1, &section);

			if(result > 0)
				size += result;
			else
				if(result < 0)
					break;
				else
					break;
		}

		return size;
	}


	bool Music::IsFinished() const {
		int status=0;
		alGetSourcei(controller, AL_SOURCE_STATE, &status);

		return status!=0;
	}


	bool Music::Play() {
		looping=false;
		alSourcei(controller, AL_LOOPING, 0);
		alSourcePlay(controller);

		return alGetError()==AL_NO_ERROR;
	}

	bool Music::Loop() {
		looping=true;

		alSourcei(controller, AL_LOOPING, 1);
		alSourcePlay(controller);

		return alGetError()==AL_NO_ERROR;
	}

	bool Music::Seek(unsigned time) {
		utils::NotImplemented();

		return false;
	}

	bool Music::IsLooping() const {
		return looping;
	}

	bool Music::Pause() {
		alSourcePause(controller);

		return alGetError()==AL_NO_ERROR;
	}

	void Music::SetVolume(float volume) {
		alSourcef(controller, AL_GAIN, volume);
	}

	float Music::GetVolume() const {
		float volume=0;
		alGetSourcef(controller, AL_GAIN, &volume);

		return volume;
	}

	Music::Music() : looping(false) {
		alGenSources(1, &controller);
	}

	Music::~Music() {
		alDeleteSources(1, &controller);
	}

	void Music::initalize() {
		alGenSources(1, &controller);
	}

}}
