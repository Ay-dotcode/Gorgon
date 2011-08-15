#include "SoundResource.h"
#include "ResourceFile.h"
#include "../External/LZMA/LzmaDecode.h"
#include "../Engine/Sound.h"

using namespace std;
using namespace gge::sound;
using namespace gge::sound::system;

namespace gge { namespace resource {
	ResourceBase *LoadSoundResource(File &File, istream &Data, int Size) {
		SoundResource *snd=new SoundResource;

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
				snd->Data=new Byte[size];

				Data.read((char*)snd->Data, size);
			} else if(gid==GID::Sound_Cmp_Props) {
				ReadFrom(Data, compression);

				if(compression==GID::LZMA) {
					compressionprops=new Byte[LZMA_PROPERTIES_SIZE];
					Data.read((char*)compressionprops,LZMA_PROPERTIES_SIZE);
				}
			} else if(gid==GID::Sound_Cmp_Wave) {
				snd->Data=new Byte[buffersize];
				snd->Size=buffersize;
				Byte *tmpdata=new Byte[size];

				Data.read((char*)tmpdata, size);
				
				size_t processed,processed2;
				CLzmaDecoderState state;
				LzmaDecodeProperties(&state.Properties,compressionprops,LZMA_PROPERTIES_SIZE);
				state.Probs = (CProb *)malloc(LzmaGetNumProbs(&state.Properties) * sizeof(CProb));

				LzmaDecode(&state,tmpdata,size,&processed,snd->Data,buffersize,&processed2);

				delete[] tmpdata;
				free(state.Probs);

				if(compressionprops)
					delete[] compressionprops;
			}
		}

		return snd;
	}

	void SoundResource::Prepare( GGEMain &main, File &file ) {
		Buffer=sound::system::CreateSoundBuffer(Format, Data, Size);
	}

} }
