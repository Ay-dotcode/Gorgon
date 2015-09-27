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
		if(!reader)				return false;
		if(!reader->TryOpen())	return false;
		if(isloaded)			return true;

		reader->Seek(entrypoint-4);
		
		auto size=reader->ReadChunkSize();

		auto ret=load(reader, size, true);

		if(ret && isloaded) {
			reader->NoLongerNeeded();
			reader.reset();
		}
	}

	bool Blob::load(std::shared_ptr<Reader> reader, unsigned long totalsize, bool forceload) {
		bool load=false;

		auto target = reader->Target(totalsize);

		entrypoint = reader->Tell();

		while(target) {
			auto gid = reader->ReadGID();
			auto size= reader->ReadChunkSize();

			GID::Type compression;

			if(gid==GID::Blob_Props) {
				reader->ReadUInt32();
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
			} else if(load && gid==GID::Blob_Cmp_Data) {
				if(size>0) {
					Encoding::Lzma.Decode(data, this->data);
				}
			}
			else {
				reader->ReadCommonChunk(*this, gid, size);
			}
		}

		return true;
	}

	bool Blob::ImportFile(const std::string &filename, int type) {
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
