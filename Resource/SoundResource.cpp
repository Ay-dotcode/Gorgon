#include "SoundResource.h"
#include "ResourceFile.h"
#include "../External/LZMA/LzmaDecode.h"

namespace gge { namespace resource {
	ResourceBase *LoadSoundResource(File* File, FILE* Data, int Size) {
		SoundResource *snd=new SoundResource;

		int tpos=ftell(Data)+Size,buffersize;
		Byte *compressionprops=NULL;
		while(ftell(Data)<tpos) {
			int gid,size,compression=0;

			fread(&gid,1,4,Data);
			fread(&size,1,4,Data);

			if(gid==GID_SOUND_PROPS) {
				fread(&buffersize,1,4,Data);
				fread(&snd->Format.Channels,1,2,Data);
				fread(&snd->Format.BlockAlign,1,2,Data);
				fread(&snd->Format.BitsPerSample,1,2,Data);
				fread(&snd->Format.SamplesPerSec,1,4,Data);
				fread(&snd->Format.AvgBytesPerSec,1,4,Data);
				fread(&snd->Format.FormatTag,1,2,Data);

				snd->Format.Size=sizeof(snd->Format);

				if(size>20)
					fseek(Data,size-20,SEEK_CUR);
			} 
			else if(gid==GID_GUID) {
				snd->guid.Load(Data);
			}
			else if(gid==GID_SOUND_WAVE) {
				snd->Size=size;
				snd->Data=new Byte[size];
				fread(snd->Data,1,size,Data);
			} else if(gid==GID_SOUND_CMP_PROPS) {
				fread(&compression,1,4,Data);
				if(compression==GID_LZMA) {
					compressionprops=new Byte[LZMA_PROPERTIES_SIZE];
					fread(compressionprops,1,LZMA_PROPERTIES_SIZE,Data);
				}
			} else if(gid==GID_SOUND_CMP_WAVE) {
				snd->Data=new Byte[buffersize];
				snd->Size=buffersize;
				Byte *tmpdata=new Byte[size];
				fread(tmpdata,1,size,Data);
				size_t processed,processed2;
				CLzmaDecoderState state;
				LzmaDecodeProperties(&state.Properties,compressionprops,LZMA_PROPERTIES_SIZE);
				state.Probs = (CProb *)malloc(LzmaGetNumProbs(&state.Properties) * sizeof(CProb));

				LzmaDecode(&state,tmpdata,size,&processed,snd->Data,buffersize,&processed2);

				delete tmpdata;
				free(state.Probs);
				delete compressionprops;
			}
		}

		return snd;
	}
} }
