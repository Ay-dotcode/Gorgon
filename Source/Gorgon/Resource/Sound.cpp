#include "Sound.h"
#include "File.h"
#include "../Encoding/FLAC.h"

namespace Gorgon { namespace Resource {

	Sound *Sound::LoadResource(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long size) {
		auto snd=new Sound;

		/*if(!snd->load(reader, size, false)) {
			delete snd;
			return nullptr;
		}*/

		return snd;
	}

	//bool Sound::Load() {
	//	if(isloaded)			return true;
	//	if(!reader)				return false;
	//	if(!reader->TryOpen())	return false;

	//	reader->Seek(entrypoint-4);

	//	auto size=reader->ReadChunkSize();

	//	auto ret=load(reader, size, true);

	//	if(ret && isloaded) {
	//		reader->NoLongerNeeded();
	//		reader.reset();
	//	}

	//	return true;
	//}


	void Sound::save(Writer& writer) {
		auto start = writer.WriteObjectStart(this);

		auto propstart = writer.WriteChunkStart(GID::Sound_Fmt);
		writer.WriteGID(compression);
		writer.WriteInt32(data.GetSampleRate());
		writer.WriteEnd(propstart);

		propstart = writer.WriteChunkStart(GID::Sound_Channels);
		writer.WriteInt8((Byte)data.GetChannelCount());
		for(unsigned i=0; i<data.GetChannelCount(); i++) {
			writer.WriteInt32((int32_t)data.GetChannelType(i));
		}
		writer.WriteEnd(propstart);

		if(compression==GID::None) {
			writer.WriteChunkHeader(
				GID::Sound_Wave, 
				data.GetBytes()
			);

			writer.WriteArray(data.RawData(), data.GetBytes());
		}
		else if(compression==GID::FLAC) {
			auto datastart = writer.WriteChunkStart(GID::Sound_Cmp_Wave);
			Encoding::Flac.Encode(data, writer.GetStream());
			writer.WriteEnd(datastart);
		}
		else {
			throw std::runtime_error("Unknown compression mode: "+String::From(compression));
		}

		writer.WriteEnd(start);
	}

	/*Sound *LoadSoundResource(File &File, std::istream &Data, int Size) {
		Sound *snd=new Sound;
		LoadSound(snd, Data, Size);
		return snd;
	}

	
	void LoadSound(Sound *snd, istream &Data, int Size) {
		encoding::LZMA lzma(false);
		
		int target=Data.tellg()+Size;
		int buffersize=0;
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
	}*/

} }
