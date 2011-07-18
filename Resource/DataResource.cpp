#include "DataResource.h"
#include "ResourceFile.h"

namespace gge { namespace resource {
	ResourceBase *LoadDataResource(File* File, FILE* Data, int Size) {
		DataResource *dat=new DataResource;
		dat->file=File;

		int tpos=ftell(Data)+Size;
		while(ftell(Data)<tpos) {
			int gid,size,tmpint;
			float tmpfloat;
			utils::Point tmppoint;
			utils::Rectangle tmprect;
			utils::SGuid tmpguid;

			char *tmpstr;
			fread(&gid,1,4,Data);
			fread(&size,1,4,Data);

			if(gid==GID::Data_Text) {
				tmpstr=new char[size+1];
				fread(tmpstr,size,1,Data);
				tmpstr[size]=0;

				dat->Add(tmpstr);
			}
			else if(gid==GID::SGuid) {
				dat->guid.Load(Data);
			}
			else if(gid==GID::Data_Int) {
				fread(&tmpint,1,4,Data);

				dat->Add(tmpint);
			}
			else if(gid==GID::Data_Float) {
				fread(&tmpfloat,1,4,Data);

				dat->Add(tmpfloat);
			}
			else if(gid==GID::Data_Point) {
				fread(&tmppoint,4,2,Data);

				dat->Add(tmppoint);
			}
			else if(gid==GID::Data_Rect) {
				fread(&tmprect,4,4,Data);

				dat->Add(tmprect);
			}
			else if(gid==GID::Data_Link) {
				tmpguid.Load(Data);

				dat->Add(tmpguid);
			}
			else if(gid==0x03300C01) {
				FontInitiator f;
				fread(&tmpint, 4,1, Data);
				fread(&size, 4,1, Data);
				f=Font::Load(File, Data,size);
				f.file=File;

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

} }
