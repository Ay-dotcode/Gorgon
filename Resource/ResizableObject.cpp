#include "ResizableObject.h"
#include "../Resource/ResourceFile.h"

namespace gre {
	ResourceBase *LoadResizableObject(ResourceFile* File, FILE* Data, int Size) {
		ResizableObjectInitiator *r=new ResizableObjectInitiator();
		int tpos=ftell(Data)+Size;

		int tmp;

		while(ftell(Data)<tpos) {
			int gid,size,tmpint;
			fread(&gid,1,4,Data);
			fread(&size,1,4,Data);

			if(gid==GID_GUID) {
				r->guid=new Guid(Data);
			}
			if(gid==GID_RESIZABLEOBJECT_PROPS) {
				r->target	= new Guid(Data);
				fread(&r->HTile, 4,1, Data);
				fread(&r->VTile, 4,1, Data);

				fread(&tmp, 4,1, Data);
				r->HIntegral.Type=(ResizableObject::IntegralSizeOption)tmp;
				fread(&tmp, 4,1, Data);
				r->VIntegral.Type=(ResizableObject::IntegralSizeOption)tmp;

				fread(&tmp, 4,1, Data);
				r->HIntegral.Overhead=(ResizableObject::IntegralSizeOption)tmp;
				fread(&tmp, 4,1, Data);
				r->VIntegral.Overhead=(ResizableObject::IntegralSizeOption)tmp;

				fread(&tmp, 4,1, Data);
				r->HIntegral.Increment=(ResizableObject::IntegralSizeOption)tmp;
				fread(&tmp, 4,1, Data);
				r->VIntegral.Increment=(ResizableObject::IntegralSizeOption)tmp;

				if(size!=(1 * 16 + 8 * 4))
					fseek(Data,size-(1 * 16 + 8 * 4),SEEK_CUR);
			} 

		}

		return r;
	}
}