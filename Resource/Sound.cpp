#include "Sound.h"
#include "File.h"
#include "../Encoding/LZMA.h"
#include "../Engine/Sound.h"

using namespace std;
using namespace gge::sound;
using namespace gge::sound::system;

namespace gge { namespace resource {

	Sound *LoadSoundResource(File &File, std::istream &Data, int Size) {
		Sound *snd=new Sound;
		LoadSound(snd, Data, Size);
		return snd;
	}

	void LoadSound(Sound *snd, istream &Data, int Size) {
		encoding::LZMA lzma(false);
		
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
					if(size>4) {
						compressionprops=new Byte[lzma.PropertySize()];
						Data.read((char*)compressionprops,lzma.PropertySize());
					}
				}
			} else if(gid==GID::Sound_Cmp_Wave) {
				//snd->Data.resize(buffersize);
				snd->Size=buffersize;
				
				if(buffersize>0) {
					if(compressionprops!=NULL)
						lzma.Decode(Data, snd->Data, compressionprops, buffersize);
					else
						encoding::Lzma.Decode(Data, snd->Data);
				}

				utils::CheckAndDeleteArray(compressionprops);
			}
		}
	}

	Sound::SoundReadError Sound::ImportWave(const std::string &filename) {
		Data.clear();
		Size=0;

		std::ifstream file(filename, std::ios::binary);

		if(!file.is_open())
			return FileNotFound;

		char sig[7]={};

		file.read(sig, 4);
		if(sig!=std::string("RIFF")) {
			return ReadError;
		}

		ReadFrom<int>(file);

		file.read(sig, 4);
		if(sig!=std::string("WAVE")) {
			return ReadError;
		}

		ReadFrom<int>(file); //fmt 
		int formatsize=ReadFrom<int>(file);
		if(formatsize!=16) {
			return ReadError;
		}

		ReadFrom(file, Format);

		ReadFrom<int>(file); //data
		ReadFrom(file, Size);
		Data.resize(Size);
		file.read((char*)(&Data[0]), Size);

    return NoError;
	}


	void Sound::Prepare( ) {
    if(!Size) return;
		Buffer=sound::system::CreateSoundBuffer(Format, &Data[0], Size);
	}

	void Sound::destroy() {
		std::vector<Byte> empty;
		Data.swap(empty);

		if(Buffer)
			sound::system::DestroySoundBuffer(Buffer);
	}

	void unpack(int &data, int nbits) {
		if(nbits==32) return;
		nbits--;

		if(data & 1<<nbits) {
			data=-(1<<nbits) + (data&~(1<<nbits));
		}
	}
	
	void pack(int &data, int nbits) {
		if(data<0) {
			nbits--;
			data=(1<<nbits) + data;
			data=data|(1<<nbits);
		}

	}

	void Sound::StereoToMono() {
		if(Format.Channels!=2 || Size==0) return;

		int bps=Format.BitsPerSample/8;		
		int samples=Size/bps;
		samples/=2;

		std::vector<gge::Byte> odata;
		Data.swap(odata);
		Data.resize(bps*samples);

		for(int i=0;i<samples;i++) {
			int l=0,r=0,v=0;
			memcpy(&l, &odata[i*2*bps], bps);
			memcpy(&r, &odata[i*2*bps+bps], bps);

			unpack(l, Format.BitsPerSample);
			unpack(r, Format.BitsPerSample);
			v=(l+r)/2;
			pack(v, Format.BitsPerSample);
			memcpy(&Data[i*bps], &v, bps);
		}

		Format.Channels=1;
		Format.BlockAlign/=2;
		Format.AvgBytesPerSec/=2;
		Size/=2;
	}

} }
