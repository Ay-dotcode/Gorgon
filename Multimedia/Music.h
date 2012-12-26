#pragma once

#include "../Engine/Sound.h"


namespace gge { namespace multimedia {


	class Music {
	public:
		bool Seek(unsigned time);
		bool IsFinished() const;
		bool Play();
		bool Loop();
		bool IsLooping() const;
		bool Pause();
		void Release();
		bool Acquire();
		void SetVolume();
		float GetVolume() const;

	protected:
		Music();
		
		system::SoundControlHandle controller; 
	};

	class StreamingMusic : public Music {
	public:

		void SetBufferSize(int size);
		int GetBufferSize() const;

		static int DefaultBufferSize;
		static int DefaultNumberOfBuffers;

	protected:

		StreamingMusic();

		virtual int decode(std::vector<Byte> &buffer)=0;
		void buffercheck();

		std::vector<Byte> buffer;
	};

	class VorbisStream : public StreamingMusic {
	public:
		VorbisStream(const std::string filename);
		VorbisStream(std::ifstream &stream);
		VorbisStream();



		bool Open(const std::string filename);
		bool Use(std::ifstream &stream);



		static int BufferSize;
	};

}}

