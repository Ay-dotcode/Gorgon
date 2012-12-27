#pragma once

#include "../Engine/Sound.h"
#include "../Engine/GGEMain.h"

struct OggVorbis_File;
struct vorbis_info;

namespace gge { namespace multimedia {


	class Music {
		friend void Initialize(GGEMain&);
	public:
		bool Seek(unsigned time);
		bool IsFinished() const;
		bool Play();
		bool Loop();
		bool IsLooping() const;
		bool Pause();
		void SetVolume(float volume);
		float GetVolume() const;

		utils::EventChain<Music> Finished;

	protected:
		Music();
		~Music();
		void initalize();
		
		sound::system::SoundControlHandle controller;
		bool looping;
	};

	class StreamingMusic : public Music {
		friend void Initialize(GGEMain&);
	public:

		void SetBufferSize(int size) { buffersize=size; }
		int GetBufferSize() const { return buffersize; }

		static int DefaultBufferSize;
		static int DefaultNumberOfBuffers;


	protected:

		StreamingMusic();
		~StreamingMusic();


		virtual int decode(std::vector<Byte> &buffer)=0;
		void buffercheck() {
			protect.Lock();
			dobuffercheck();
			protect.Unlock();
		}
		void dobuffercheck();
		void recreatebuffers();

		static void CheckAll();

		std::vector<Byte> data;
		int buffersize;
		std::vector<sound::system::SoundBufferHandle> buffers;

		int format, freq;
		bool buffersattached;

		os::Mutex protect;

		static std::vector<StreamingMusic*> members;
	};

	class VorbisStream : public StreamingMusic {
		friend size_t ogg_read(void* dest, size_t size1, size_t size2, void* vs);
		friend long ogg_tell( void *vs );
		friend int ogg_seek( void *vs, long long to, int type );
	public:
		VorbisStream(const std::string filename);
		VorbisStream(std::ifstream &stream);
		VorbisStream();
		~VorbisStream();



		bool Open(const std::string filename);
		void Use(std::istream &stream);
		void Close();


	protected:	
		std::istream	*stream;
		OggVorbis_File  *ogg;
		vorbis_info     *info;
		bool cleanstream;

		virtual int decode(std::vector<Byte> &buffer);

		void init();
		void setupread();

	};

}}

