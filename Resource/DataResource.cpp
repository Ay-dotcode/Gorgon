#include "DataResource.h"
#include "ResourceFile.h"

using namespace std;
using namespace gge::utils;

namespace gge { namespace resource {
	ResourceBase *LoadDataResource(File &File, std::istream &Data, int Size) {
		DataResource *dat=new DataResource;
		dat->file=&File;

		int target=Data.tellg()+Size;
		while(Data.tellg()<target) {
			int gid,size;

			ReadFrom(Data, gid);
			ReadFrom(Data, size);

			utils::SGuid tmpguid;

			char *tmpstr;

			if(gid==GID::Data_Text) {
				tmpstr=new char[size];
				Data.read(tmpstr,size);

				dat->Add(string(tmpstr,size));
			}
			else if(gid==GID::SGuid) {
				dat->guid.Load(Data);
			}
			else if(gid==GID::Guid) {
				dat->guid.LoadLong(Data);
			}
			else if(gid==GID::Data_Int) {
				dat->Add(ReadFrom<int>(Data));
			}
			else if(gid==GID::Data_Float) {
				dat->Add(ReadFrom<float>(Data));
			}
			else if(gid==GID::Data_Point) {
				dat->Add(ReadFrom<Point>(Data));
			}
			else if(gid==GID::Data_Rect) {
				dat->Add(ReadFrom<Rectangle>(Data));
			}
			else if(gid==GID::Data_Link) {
				tmpguid.Load(Data);

				dat->Add(tmpguid);
			}
			else if(gid==GID::Data_Color) {
				dat->Add(ReadFrom<int>(Data));
			}
			else if(gid==GID::Data_Font) {
				FontInitiator f;
				ReadFrom<int>(Data);//useless gid
				f=Font::Load(File, Data, ReadFrom<int>(Data));
				f.file=&File;

				dat->Add(f);
			}
			else ///!TODO: should query for pluggable data items
				EatChunk(Data,size);
		}

		return dat;
	}

	
	void LinkData::Prepare(File *File) { 
		value=File->Root().FindObject(guid);
	}


	IntegerData		& DataResource::Add( int value ) {
		IntegerData &o=*new IntegerData(value);
		Data.Add(o);

		return o;
	}

	FloatData		& DataResource::Add( float value ) {
		FloatData &o=*new FloatData(value);
		Data.Add(o);

		return o;
	}

	StringData		& DataResource::Add( string value ) {
		StringData &o=*new StringData(value);
		Data.Add(o);

		return o;
	}

	PointData		& DataResource::Add( utils::Point value ) {
		PointData &o=*new PointData(value);
		Data.Add(o);

		return o;
	}

	RectangleData	& DataResource::Add( utils::Rectangle value ) {
		RectangleData &o=*new RectangleData(value);
		Data.Add(o);

		return o;
	}

	FontData		& DataResource::Add( Font value ) {
		FontData &o=*new FontData(value);
		Data.Add(o);

		return o;
	}

	FontData		& DataResource::Add( FontInitiator value ) {
		FontData &o=*new FontData(value);
		Data.Add(o);

		return o;
	}

	LinkData		& DataResource::Add( utils::SGuid value ) {
		LinkData &o=*new LinkData(value);
		Data.Add(o);

		return o;
	}

	void DataResource::Prepare( GGEMain &main ) {
		for(utils::Collection<IData>::Iterator i=Data.First();i.isValid();i.Next()) 
			i->Prepare(file);
	}

} }
