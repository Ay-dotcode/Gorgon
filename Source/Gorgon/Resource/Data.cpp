#include "Data.h"


#include "File.h"

namespace Gorgon { namespace Resource {
	
	void Data::save(Writer& writer) { 
		auto start=writer.WriteObjectStart(this);
		
		for(auto &item : items) {
			item.Save(writer);
		}
		
		writer.WriteEnd(start);
	}
	
	Data *Data::LoadResource(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long totalsize) {

		Data *obj=new Data;
		auto target = reader->Target(totalsize);
		
		while(!target) {
			auto gid = reader->ReadGID();
			auto size= reader->ReadChunkSize();
			
			if(!reader->ReadCommonChunk(*obj, gid, size)) {
				bool done=false;
				
				if(DataLoaders.count(gid)) {
					auto data=DataLoaders[gid](file, reader, size);
					if(data) {
						obj->items.Add(data);
					}
				}
				else {
					Utils::ASSERT_FALSE("Unknown chunk: "+String::From(gid));
					reader->EatChunk(size);
				}
			}
		}
		
		return obj;
	}
	
	void Data::InitializeLoaders() {
		DataLoaders[GID::Data_Int] = &IntegerData::Load;
	}
	
	DataItem* IntegerData::Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, long unsigned int totalsize) { 
#ifndef NDEBUG
		auto target = reader->Target(totalsize);
#endif
		
		std::string name=reader->ReadString();
		int number = reader->ReadInt32();
		
		ASSERT((bool)target, "Integer data size mismatch: should be 4, reported as "+String::From(totalsize));
		
		return new IntegerData(name, number);
	}

	
	std::map<GID::Type, Data::LoaderFn> Data::DataLoaders;

} }
