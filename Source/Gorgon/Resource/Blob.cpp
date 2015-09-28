#include "Blob.h"
#include "File.h"
#include "../Encoding/LZMA.h"

namespace Gorgon { namespace Resource {

	Blob *Blob::LoadResource(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long size) {
		auto blob=new Blob;

		if(!blob->load(reader, size, false)) {
			delete blob;
			return nullptr;
		}

		return blob;
	}

	std::vector<Byte>& Blob::Ready(unsigned long size, Type type) {
		data.resize(0);
		data.resize(size);

		this->type=type;
		isloaded=true;

		if(reader) {
			reader->NoLongerNeeded();
			reader.reset();
		}

		return data;
	}

	bool Blob::Load() {
		if(isloaded)			return true;
		if(!reader)				return false;
		if(!reader->TryOpen())	return false;

		reader->Seek(entrypoint-4);
		
		auto size=reader->ReadChunkSize();

		auto ret=load(reader, size, true);

		if(ret && isloaded) {
			reader->NoLongerNeeded();
			reader.reset();
		}
		
		return true;
	}

	bool Blob::load(std::shared_ptr<Reader> reader, unsigned long totalsize, bool forceload) {
		bool load=false;

		auto target = reader->Target(totalsize);

		entrypoint = reader->Tell();

		unsigned long uncompressed = 0;
		while(!target) {
			auto gid = reader->ReadGID();
			auto size= reader->ReadChunkSize();

			GID::Type compression;

			if(gid==GID::Blob_Props) {
				uncompressed=reader->ReadUInt32();
				compression=reader->ReadGID();
				type=reader->ReadInt32();

				load=reader->ReadBool() || forceload;
				if(!load) {
					reader->KeepOpen();
					this->reader=reader;
				}
			}
			else if(load && gid==GID::Blob_Data) {
				reader->ReadArray(&data[0], size);

				isloaded=true;
			} else if(load && gid==GID::Blob_Cmp_Data) {
				if(size>0) {
					Encoding::Lzma.Decode(reader->GetStream(), data, nullptr, uncompressed);
				}

				isloaded=true;
			}
			else {
				if(!reader->ReadCommonChunk(*this, gid, size)) {
					Utils::ASSERT_FALSE("Unknown chunk: "+String::From(gid));
					reader->EatChunk(size);
				}
			}
		}

		return true;
	}

	bool Blob::ImportFile(const std::string &filename, Type type) {
		data.clear();
		this->type=type;

		std::ifstream file(filename, std::ios::binary);

		if(!file.is_open())
			return false;

		Byte buff[1024];
		while(true) {
			file.read((char*)buff, 1024);
			unsigned size=(unsigned)file.gcount();
			if(!size) break;
			data.resize(data.size()+size);
			memcpy(&data[data.size()-size],buff,size);
		}

		isloaded=true;
		if(reader) {
			reader->NoLongerNeeded();
			reader.reset();
		}

		
		return true;
	}

	bool Blob::AppendFile(const std::string &filename) {
		std::ifstream file(filename, std::ios::binary);

		if(!file.is_open())
			return false;

		Byte buff[1024];
		while(true) {
			file.read((char*)buff, 1024);
			unsigned size=(unsigned)file.gcount();
			if(!size) break;
			data.resize(data.size()+size);
			memcpy(&data[data.size()-size],buff,size);
		}
		
		return true;
	}


} }
