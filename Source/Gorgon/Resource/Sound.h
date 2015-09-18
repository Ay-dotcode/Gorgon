#pragma once

#include "GRE.h"
#include "Base.h"
#include "../Engine/Sound.h"
#include "../Multimedia/Wave.h"

namespace gge { namespace resource {
	class File;
	class Sound;
	
	////This function loads a sound resource from the given file
	Sound *LoadSoundResource(File &File, std::istream &Data, int Size);

	////This function loads a sound resource from the given file
	void LoadSound(Sound *snd, std::istream &Data, int Size);

	////This is sound resource. It may contain 22kHz or 44kHz mono or stereo wave files.
	/// Also supports LZMA compression. No native sound compression is supported.
	class Sound : public Base {
		friend void LoadSound(Sound *snd, std::istream &Data, int Size);
	public:
    enum SoundReadError {
      NoError=0,
      ReadError=1, 
      FileNotFound
    };
		////04010000h (Extended, Sound)
		virtual GID::Type GetGID() const { return GID::Sound; }

		////Wave data
		std::vector<Byte> Data;
		////Format
		gge::sound::system::WaveFormat Format;
		////Size of the data
		int Size;

		Sound() { Buffer=0; Size=0; Format.BitsPerSample=Format.Channels=0; }

		operator gge::sound::system::SoundBufferHandle() {
			return Buffer;
		}

		
		////Destroys used data
		void destroy();
		////Destroys used data
		virtual ~Sound() { destroy(); }

		SoundReadError ImportWave(const std::string &filename);

		void StereoToMono();

		////When this file is prepared to be used, this value will be used to store sound buffer
		gge::sound::system::SoundBufferHandle Buffer;

		gge::sound::Wave *CreateWave() { return new gge::sound::Wave(Buffer); }

		gge::sound::Wave *Create3DWave(float maxWaveDistance=-1) { 
			if(maxWaveDistance==-1) 
				maxWaveDistance=float(Main.BoundingBox.Width()+Main.BoundingBox.Height());

			return new gge::sound::Wave(Buffer,	maxWaveDistance); 
		}

		virtual void Prepare(GGEMain &main, File &file) { Prepare(); }

		void Prepare();
	};
} }
