#include "DataResource.h"
#include "ResourceFile.h"

namespace gge { namespace resource {
	ResourceBase *LoadDataResource(ResourceFile* File, FILE* Data, int Size) {
		DataResource *dat=new DataResource;
		dat->file=File;

		int tpos=ftell(Data)+Size;
		while(ftell(Data)<tpos) {
			int gid,size,tmpint;
			float tmpfloat;
			Point tmppoint;
			gge::Rectangle tmprect;
			Guid *tmpguid=NULL;

			char *tmpstr;
			fread(&gid,1,4,Data);
			fread(&size,1,4,Data);

			if(gid==GID_DATAARRAY_TEXT) {
				tmpstr=new char[size+1];
				fread(tmpstr,size,1,Data);
				tmpstr[size]=0;

				dat->Add(tmpstr);
			}
			else if(gid==GID_GUID) {
				dat->guid=new Guid(Data);
			}
			else if(gid==GID_DATAARRAY_INT) {
				fread(&tmpint,1,4,Data);

				dat->Add(tmpint);
			}
			else if(gid==GID_DATAARRAY_FLOAT) {
				fread(&tmpfloat,1,4,Data);

				dat->Add(tmpfloat);
			}
			else if(gid==GID_DATAARRAY_POINT) {
				fread(&tmppoint,1,8,Data);

				dat->Add(tmppoint);
			}
			else if(gid==GID_DATAARRAY_RECT) {
				fread(&tmprect,1,16,Data);

				dat->Add(tmprect);
			}
			else if(gid==GID_DATAARRAY_LINK) {
				tmpguid=new Guid(Data);

				dat->Add(tmpguid);

				delete tmpguid;
				tmpguid=NULL;
			}
			else if(gid==0x03300C01) {
				FontInitiator f;
				fread(&tmpint, 4,1, Data);
				fread(&size, 4,1, Data);
				f=Font::Load(File, Data,size);
				f.file=File;

				dat->Add(f);
			}
			else //should query for pluginable data items
				EatChunk(Data,size);
		}

		return dat;
	}

	
	void LinkData::Prepare(ResourceFile *File) { 
		value=File->Root().FindObject(guid);
	}

} }
