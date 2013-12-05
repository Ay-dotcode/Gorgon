#include "Blob.h"
#include "File.h"
#include "../Encoding/LZMA.h"

using namespace std;

namespace gge { namespace resource {

	Blob *LoadBlobResource(File &File, std::istream &Data, int Size) {
		Blob *blob=new Blob;
		LoadBlob(blob, Data, Size);
		return blob;
	}

	void LoadBlob(Blob *blob, istream &Data, int Size) {
		encoding::LZMA lzma(false);
		
		int target=Data.tellg()+Size;
		int buffersize;

		while(Data.tellg()<target) {
			int gid,size,compression=0;
			ReadFrom(Data, gid);
			ReadFrom(Data, size);

			if(gid==GID::Blob_Props) {
				ReadFrom(Data, buffersize);
				ReadFrom(Data, compression);
				ReadFrom(Data, blob->Type);
				blob->LateLoading=ReadFrom<int>(Data)!=0;
			} 
			else if(gid==GID::Guid) {
				blob->guid.LoadLong(Data);
			}
			else if(gid==GID::SGuid) {
				blob->guid.Load(Data);
			}
			else if(gid==GID::Blob_Data) {
				blob->data.resize(size);

				Data.read((char*)&blob->data[0], size);
			} else if(gid==GID::Blob_Cmp_Data) {
				if(buffersize>0) {
					encoding::Lzma.Decode(Data, blob->data);
				}
			}
		}
	}

	bool Blob::ImportFile(const std::string &filename, int type) {
		data.clear();
		Type=type;

		std::ifstream file(filename, std::ios::binary);

		if(!file.is_open())
			return false;

		Byte buff[1024];
		while(true) {
			file.read((char*)buff, 1024);
			unsigned size=file.gcount();
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
			unsigned size=file.gcount();
			if(!size) break;
			data.resize(data.size()+size);
			memcpy(&data[data.size()-size],buff,size);
		}
		
		return true;
	}


} }
