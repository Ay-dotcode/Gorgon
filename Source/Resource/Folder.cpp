#include "Folder.h"

namespace Gorgon { namespace Resource {

	void Folder::Prepare() {
		Base::Prepare();

		if(reallyloadnames) {
			for(auto &child : children) {
				if(child.GetName()!="") {
					namedlist.insert(std::make_pair(child.GetName(), &child));
				}
			}
		}
	}

	Folder *LoadFolderResource(std::istream &data, int size, File &file, bool onlyfirst, bool shallow) {
		return nullptr;
	}


} }

#ifdef nononone

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

} }
#endif