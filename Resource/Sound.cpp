#include "Sound.h"
#include "File.h"
#include "../Encoding/LZMA.h"
#include "../Engine/Sound.h"

using namespace std;
using namespace gge::sound;
using namespace gge::sound::system;

namespace gge { namespace resource {
	Sound *LoadSoundResource(File &File, istream &Data, int Size) {
		Sound *snd=new Sound;
		encoding::LZMA Lzma(false);
		
		int target=Data.tellg()+Size;
		int buffersize;
		Byte *compressionprops=NULL;

		while(Data.tellg()<target) {
			int gid,size,compression=0;
			ReadFrom(Data, gid);
			ReadFrom(Data, size);

			if(gid==GID::Sound_Props) {
				ReadFrom(Data, buffersize);
				ReadFrom(Data, snd->Format.Channels);
				ReadFrom(Data, snd->Format.BlockAlign);
				ReadFrom(Data, snd->Format.BitsPerSample);
				ReadFrom(Data, snd->Format.SamplesPerSec);
				ReadFrom(Data, snd->Format.AvgBytesPerSec);
				ReadFrom(Data, snd->Format.FormatTag);

				snd->Format.Size=sizeof(snd->Format);

				if(size>20)
					Data.seekg(size-20,ios::cur);
			} 
			else if(gid==GID::Guid) {
				snd->guid.LoadLong(Data);
			}
			else if(gid==GID::SGuid) {
				snd->guid.Load(Data);
			}
			else if(gid==GID::Sound_Wave) {
				snd->Size=size;
				snd->Data.resize(size);

				Data.read((char*)&snd->Data[0], size);
			} else if(gid==GID::Sound_Cmp_Props) {
				ReadFrom(Data, compression);

				if(compression==GID::LZMA) {
					compressionprops=new Byte[Lzma.PropertySize()];
					Data.read((char*)compressionprops,Lzma.PropertySize());
				}
			} else if(gid==GID::Sound_Cmp_Wave) {
				//snd->Data.resize(buffersize);
				snd->Size=buffersize;

				Lzma.Decode(Data, snd->Data, compressionprops, buffersize);

				utils::CheckAndDeleteArray(compressionprops);
			}
		}

		return snd;
	}

	void Sound::Prepare( GGEMain &main, File &file ) {
		Buffer=sound::system::CreateSoundBuffer(Format, &Data[0], Size);
	}

	void Sound::destroy() {
		std::vector<Byte> empty;
		Data.swap(empty);

		if(Buffer)
			sound::system::DestroySoundBuffer(Buffer);
	}

} }
