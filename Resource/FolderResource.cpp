#include "FolderResource.h"
#include "ResourceFile.h"
#include "../Utils/BufferList.h"


#include "ResourceBase.h"
#include "TextResource.h"
#include "ImageResource.h"
#include "DataResource.h"
#include "SoundResource.h"
#include "AnimationResource.h"
#include "BitmapFontResource.h"



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
					fold->Subitems.AddItem(obj, fold->Subitems.HighestOrder()+1);

					/*if(captions.getCount()>curcapname) {
						obj->caption=captions[curcapname];

						obj->name=names[curcapname];
					}*/
				} else
					DisplayMessage("Folder Resource","Unknown Node!!");

				curcapname++;
			}

		}

		return fold;
	}

	
		////Returns the given subitem with folder resource type. Used to avoid type casting
	FolderResource	*FolderResource::asFolder	(int Index) { 
#ifdef _DEBUG
			if(Subitems[Index]->getGID()!=GID_FOLDER) {
				DisplayMessage("Folder Resource","Non folder item requested as folder!");
				assert(0);
			}
#endif
			return dynamic_cast<FolderResource*>(Subitems[Index]); 
		}
	TextResource	*FolderResource::asText		(int Index) { 
#ifdef _DEBUG
		if(Subitems[Index]->getGID()!=GID_TEXT) {
			DisplayMessage("Folder Resource","Non text item requested as text!");
			assert(0);
		}
#endif
		return dynamic_cast<TextResource*>(Subitems[Index]); 
	}
	ImageResource	*FolderResource::asImage	(int Index) { 
#ifdef _DEBUG
		if(Subitems[Index]->getGID()!=GID_IMAGE) {
			DisplayMessage("Folder Resource","Non image item requested as image!");
			assert(0);
		}
#endif
		return dynamic_cast<ImageResource*>(Subitems[Index]); 
	}
	DataResource	*FolderResource::asData		(int Index) { 
#ifdef _DEBUG
		if(Subitems[Index]->getGID()!=GID_DATAARRAY) {
			DisplayMessage("Folder Resource","Non data item requested as data!");
			assert(0);
		}
#endif
		return dynamic_cast<DataResource*>(Subitems[Index]); 
	}
	SoundResource	*FolderResource::asSound	(int Index) { 
#ifdef _DEBUG
		if(Subitems[Index]->getGID()!=GID_SOUND) {
			DisplayMessage("Folder Resource","Non sound item requested as sound!");
			assert(0);
		}
#endif
		return dynamic_cast<SoundResource*>(Subitems[Index]); 
	}
	AnimationResource	*FolderResource::asAnimation	(int Index) { 
#ifdef _DEBUG
		if(Subitems[Index]->getGID()!=GID_ANIMATION) {
			DisplayMessage("Folder Resource","Non animation item requested as animation!");
			assert(0);
		}
#endif
		return dynamic_cast<AnimationResource*>(Subitems[Index]); 
	}
	BitmapFontResource	*FolderResource::asBitmapFont	(int Index) { 
#ifdef _DEBUG
			if(Subitems[Index]->getGID()!=GID_FONT) {
				DisplayMessage("Folder Resource","Non bitmap font item requested as bitmap font!");
				assert(0);
			}
#endif
			return dynamic_cast<BitmapFontResource*>(Subitems[Index]); 
		}

}