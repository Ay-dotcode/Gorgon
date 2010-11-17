#include "FolderResource.h"
#include "ResourceFile.h"
#include "../Utils/BufferList.h"

namespace gre {

	ResourceBase *LoadFolderResource(ResourceFile* File, FILE* Data, int Size) {
		char *tmpstr;
		int tpos=ftell(Data)+Size;
		int i;
		int curcapname=0;
		FolderResource *fold=new FolderResource();
		fold->EntryPoint=ftell(Data);

		BufferList<char> captions;
		BufferList<char> names;

		while(ftell(Data)<tpos) {
			int gid, size;
			fread(&gid,1,4,Data);
			fread(&size,1,4,Data);

			if(gid==GID_FOLDER_NAMES) {
				///*Names and captions will not be loaded to preserve memory
				/*int tpos=ftell(Data)+size;

				while(ftell(Data)<tpos) {
					int gid,size;
					fread(&gid,1,4,Data);
					fread(&size,1,4,Data);

					if(gid==GID_FOLDER_NAME) {
						if(size) {
							tmpstr=new char[size+1];
							fread(tmpstr,size,1,Data);
							tmpstr[size]=0;

							for(i=0;i<strlen(tmpstr);i++) {
								if(tmpstr[i]==0x01) {
									tmpstr[i]=0;
									names.Add(tmpstr,i+1);
									captions.Add(tmpstr+i+1,size-i);
								}
							}

							delete tmpstr;
						}
					}
					else {
						fseek(Data,size,SEEK_CUR);
					}
				}*/

				EatChunk(Data, size);
			} else if(gid==GID_GUID) {
				fold->guid=new Guid(Data);
			} else {
				///*Load sub resource
				ResourceBase *obj=File->LoadObject(Data,gid,size);

				if(obj!=NULL) {
					fold->Subitems.AddItem(obj);

					/*if(captions.getCount()>curcapname) {
						obj->caption=new char[captions.getSize(curcapname)];
						memcpy(obj->caption,captions[curcapname],captions.getSize(curcapname));

						obj->name=new char[names.getSize(curcapname)];
						memcpy(obj->name,names[curcapname],names.getSize(curcapname));
					}*/
				} else
					DisplayMessage("Folder Resource","Unknown Node!!");

				curcapname++;
			}

		}

		return fold;
	}
}