#include "Blob.h"
#include "File.h"
#include "../Encoding/LZMA.h"

namespace Gorgon { namespace Resource {

	Blob *Blob::LoadResource(File &file, std::istream &data, unsigned long size) {
		auto blob=new Blob;

		blob->file=file.Self();
		if(!blob->load(data, size, false)) {
			delete blob;
			return nullptr;
		}

		return blob;
	}

	bool Blob::Load() {
		auto f=this->file.lock();
		if(!f) {
			return false;
		}

		auto &file=*f;

		std::ifstream &data=file.open();

		file.Seek(entrypoint-4);
		
		auto size=file.ReadChunkSize();

		return load(data, size, true);
	}

	bool Blob::load(std::istream &data, unsigned long totalsize, bool forceload) {
		bool load=false;

		auto target = data.tellg()+totalsize;

		entrypoint = (unsigned long)data.tellg();

		auto f=this->file.lock();
		if(!f) {
			return false;
		}

		auto &file=*f;

		while(data.tellg()<target) {
			auto gid = file.ReadGID();
			auto size= file.ReadChunkSize();

			GID::Type compression;

			if(gid==GID::Blob_Props) {
				file.ReadUInt32();
				compression=file.ReadGID();
				type=file.ReadInt32();

				load=file.ReadBool();
				if(!load) file.KeepOpen();
			}
			else if(load && gid==GID::Blob_Data) {
				data.read((char*)&this->data[0], size);
			} else if(load && gid==GID::Blob_Cmp_Data) {
				if(size>0) {
					Encoding::Lzma.Decode(data, this->data);
				}
			}
			else {
				file.LoadChunk(*this, data, gid, size, true);
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
