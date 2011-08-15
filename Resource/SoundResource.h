#pragma once

#include "GRE.h"
#include "ResourceBase.h"
#include "../Engine/Sound.h"
#include "../Engine/Wave.h"

namespace gge { namespace resource {
	class File;
	
	////This function loads a sound resource from the given file
	ResourceBase *LoadSoundResource(File &File, std::istream &Data, int Size);

	////This is sound resource. It may contain 22kHz or 44kHz mono or stereo wave files.
	/// Also supports LZMA compression. No native sound compression is supported.
	class SoundResource : public ResourceBase {
		friend ResourceBase *LoadSoundResource(File &File, std::istream &Data, int Size);
	public:
		////04010000h (Extended, Sound)
		virtual GID::Type getGID() const { return GID::Sound; }
		////Currently does nothing
		virtual bool Save(File &File, std::ostream &Data) { return false; }

		////Wave data
		Byte *Data;
		////Format
		gge::sound::system::WaveFormat Format;
		////Size of the data
		int Size;

		SoundResource() { Buffer=NULL; Data=NULL; Size=0; Format.BitsPerSample=Format.Channels=0; }

		operator gge::sound::system::SoundBufferHandle() {
			return Buffer;
		}

		////Destroys used data
		void destroy() { if(Data) delete Data; }
		////Destroys used data
		virtual ~SoundResource() { destroy(); ResourceBase::~ResourceBase(); }

		////When this file is prepared to be used, this value will be used to store sound buffer
		gge::sound::system::SoundBufferHandle Buffer;

		gge::sound::Wave *CreateWave() { return new gge::sound::Wave(Buffer); }

		gge::sound::Wave *Create3DWave(float maxWaveDistance) { return new gge::sound::Wave(Buffer,	maxWaveDistance); }

		virtual void Prepare(GGEMain &main, File &file);
	};
} }
