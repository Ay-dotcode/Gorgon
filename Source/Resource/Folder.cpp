#include "Folder.h"
#include "File.h"
#include "../Utils/BufferList.h"


#include "Base.h"
#include "Text.h"
#include "Image.h"
#include "DataArray.h"
#include "Sound.h"
#include "Animation.h"
#include "BitmapFont.h"

using namespace std;
using namespace gge::utils;

namespace gge { namespace resource {

	Folder *LoadFolderResource(File &File, istream &Data, int Size, bool LoadNames, bool OnlyFirst) {
		int targetpos=Data.tellg()+Size;
		char Namebuffer[256];
		vector<string> names;
		vector<string>::iterator namec=names.begin();

		Folder *fold=new Folder();
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
				Folder *obj=LoadFolderResource(File, Data, size, LoadNames);

				fold->Subitems.Add(obj, fold->Subitems.HighestOrder()+1);
				if(LoadNames && namec!=names.end()) {
					obj->name=*namec;
					++namec;
				}

			}
			else {
				///*Load sub resource
				Base *obj=File.LoadObject(Data,gid,size);

				if(obj!=NULL) {
					fold->Subitems.Add(obj, fold->Subitems.HighestOrder()+1);
					if(LoadNames && fold->reallyloadnames && namec!=names.end()) {
						obj->name=*namec;
						++namec;
					}

					if(OnlyFirst)
						break;

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
	Folder	*Folder::asFolder	(int Index) { 
#ifdef _DEBUG
			if(Subitems[Index].GetGID()!=GID::Folder) {
				os::DisplayMessage("Folder Resource","Non folder item requested as folder!");
				assert(0);
			}
#endif
			return dynamic_cast<Folder*>(&Subitems[Index]); 
		}
	Text	*Folder::asText		(int Index) { 
#ifdef _DEBUG
		if(Subitems[Index].GetGID()!=GID::Text) {
			os::DisplayMessage("Folder Resource","Non text item requested as text!");
			assert(0);
		}
#endif
		return dynamic_cast<Text*>(&Subitems[Index]); 
	}
	Image	*Folder::asImage	(int Index) { 
#ifdef _DEBUG
		if(Subitems[Index].GetGID()!=GID::Image) {
			os::DisplayMessage("Folder Resource","Non image item requested as image!");
			assert(0);
		}
#endif
		return dynamic_cast<Image*>(&Subitems[Index]); 
	}
	DataArray	*Folder::asData		(int Index) { 
#ifdef _DEBUG
		if(Subitems[Index].GetGID()!=GID::Data) {
			os::DisplayMessage("Folder Resource","Non data item requested as data!");
			assert(0);
		}
#endif
		return dynamic_cast<DataArray*>(&Subitems[Index]); 
	}
	Sound	*Folder::asSound	(int Index) { 
#ifdef _DEBUG
		if(Subitems[Index].GetGID()!=GID::Sound) {
			os::DisplayMessage("Folder Resource","Non sound item requested as sound!");
			assert(0);
		}
#endif
		return dynamic_cast<Sound*>(&Subitems[Index]); 
	}
	Animation	*Folder::asAnimation	(int Index) { 
#ifdef _DEBUG
		if(Subitems[Index].GetGID()!=GID::Animation) {
			os::DisplayMessage("Folder Resource","Non animation item requested as animation!");
			assert(0);
		}
#endif
		return dynamic_cast<Animation*>(&Subitems[Index]); 
	}
	BitmapFont	*Folder::asBitmapFont	(int Index) { 
#ifdef _DEBUG
			if(Subitems[Index].GetGID()!=GID::Font) {
				os::DisplayMessage("Folder Resource","Non bitmap font item requested as bitmap font!");
				assert(0);
			}
#endif
			return dynamic_cast<BitmapFont*>(&Subitems[Index]); 
		}

	void Folder::Prepare(GGEMain &main, File &file) {
		Base::Prepare(main, file);
		
		for(auto it=Subitems.First();it.IsValid();it.Next()) {
			if(it->name!="")
				namedlist.insert(std::pair<std::string, Base*>(it->name,it.CurrentPtr()));
		}
	}

} }
