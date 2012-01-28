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

using namespace std;
using namespace gge::utils;

namespace gge { namespace resource {

	FolderResource *LoadFolderResource(File &File, istream &Data, int Size, bool LoadNames) {
		int targetpos=Data.tellg()+Size;
		char Namebuffer[256];
		vector<string> names;
		vector<string>::iterator namec=names.begin();

		FolderResource *fold=new FolderResource();
		fold->EntryPoint=(int)Data.tellg(); //forced to int because the system uses only 32 bits for size

		while(Data.tellg()<targetpos) {
			int gid, size;
			ReadFrom(Data, gid);
			ReadFrom(Data, size);

			if(gid==GID::Folder_Names) {
				if(LoadNames && fold->reallyloadnames) {
					int targetpos=Data.tellg()+size;
					while(Data.tellg()<targetpos) {
						GID::Type gid; 
						int size;
						ReadFrom(Data, gid);
						ReadFrom(Data, size);
						
						if(size>255) {
							Data.read(Namebuffer, 255);
							Namebuffer[255]=0;
							EatChunk(Data, size-255);
						}
						else {
							Data.read(Namebuffer, size);
							Namebuffer[size]=0;
						}

						string s=Namebuffer;
						int l=s.find('\x01');
						s=s.substr(0,l);
						names.push_back(s);
					}

					namec=names.begin();
				}
				else {
					EatChunk(Data, size);
				}
			} 
			else if(gid==GID::Guid) {
				fold->guid.LoadLong(Data);
			} 
			else if(gid==GID::SGuid) {
				fold->guid.Load(Data);
			} 
			else if(gid==GID::Folder_Props) {
				fold->reallyloadnames=ReadFrom<int>(Data) ? true : false;

				EatChunk(Data, size-4);
			}
			else if(gid==GID::Folder) {
				FolderResource *obj=LoadFolderResource(File, Data, size, LoadNames);

				fold->Subitems.Add(obj, fold->Subitems.HighestOrder()+1);
				if(LoadNames && namec!=names.end()) {
					obj->name=*namec;
					++namec;
				}

			}
			else {
				///*Load sub resource
				ResourceBase *obj=File.LoadObject(Data,gid,size);

				if(obj!=NULL) {
					fold->Subitems.Add(obj, fold->Subitems.HighestOrder()+1);
					if(LoadNames && fold->reallyloadnames && namec!=names.end()) {
						obj->name=*namec;
						++namec;
					}

				} else {
#ifdef _DEBUG
					os::DisplayMessage("Folder Resource","Unknown Node!!");
#endif
				}
			}

		}

		return fold;
	}

	
		////Returns the given subitem with folder resource type. Used to avoid type casting
	FolderResource	*FolderResource::asFolder	(int Index) { 
#ifdef _DEBUG
			if(Subitems[Index].getGID()!=GID::Folder) {
				os::DisplayMessage("Folder Resource","Non folder item requested as folder!");
				assert(0);
			}
#endif
			return dynamic_cast<FolderResource*>(&Subitems[Index]); 
		}
	TextResource	*FolderResource::asText		(int Index) { 
#ifdef _DEBUG
		if(Subitems[Index].getGID()!=GID::Text) {
			os::DisplayMessage("Folder Resource","Non text item requested as text!");
			assert(0);
		}
#endif
		return dynamic_cast<TextResource*>(&Subitems[Index]); 
	}
	ImageResource	*FolderResource::asImage	(int Index) { 
#ifdef _DEBUG
		if(Subitems[Index].getGID()!=GID::Image) {
			os::DisplayMessage("Folder Resource","Non image item requested as image!");
			assert(0);
		}
#endif
		return dynamic_cast<ImageResource*>(&Subitems[Index]); 
	}
	DataResource	*FolderResource::asData		(int Index) { 
#ifdef _DEBUG
		if(Subitems[Index].getGID()!=GID::Data) {
			os::DisplayMessage("Folder Resource","Non data item requested as data!");
			assert(0);
		}
#endif
		return dynamic_cast<DataResource*>(&Subitems[Index]); 
	}
	SoundResource	*FolderResource::asSound	(int Index) { 
#ifdef _DEBUG
		if(Subitems[Index].getGID()!=GID::Sound) {
			os::DisplayMessage("Folder Resource","Non sound item requested as sound!");
			assert(0);
		}
#endif
		return dynamic_cast<SoundResource*>(&Subitems[Index]); 
	}
	AnimationResource	*FolderResource::asAnimation	(int Index) { 
#ifdef _DEBUG
		if(Subitems[Index].getGID()!=GID::Animation) {
			os::DisplayMessage("Folder Resource","Non animation item requested as animation!");
			assert(0);
		}
#endif
		return dynamic_cast<AnimationResource*>(&Subitems[Index]); 
	}
	BitmapFontResource	*FolderResource::asBitmapFont	(int Index) { 
#ifdef _DEBUG
			if(Subitems[Index].getGID()!=GID::Font) {
				os::DisplayMessage("Folder Resource","Non bitmap font item requested as bitmap font!");
				assert(0);
			}
#endif
			return dynamic_cast<BitmapFontResource*>(&Subitems[Index]); 
		}

	void FolderResource::Prepare(GGEMain &main, File &file) {
		ResourceBase::Prepare(main, file);
		
		for(auto it=Subitems.First();it.isValid();it.Next()) {
			if(it->name!="")
				namedlist.insert(std::pair<std::string, ResourceBase*>(it->name,it.CurrentPtr()));
		}
	}

} }
