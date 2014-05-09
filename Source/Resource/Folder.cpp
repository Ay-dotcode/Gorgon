#include "Folder.h"
#include "File.h"

namespace Gorgon { namespace Resource {

	Folder::Folder(File &file) {
		this->file=file.Self();
	}

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

	bool Folder::load(std::istream &data, unsigned long totalsize, bool onlyfirst, bool shallow, bool load) {
		unsigned long target = data.tellg()+totalsize;

		auto f=this->file.lock();
		if(!f) {
			return false;
		}

		auto &file=*f;

		while(data.tellg()<target) {
			auto gid = file.ReadGID();
			auto size= file.ReadUInt32();

			if(gid==GID::Folder_Names) {
				file.EatChunk(size);
			}
			else if(gid==GID::Name) {
				if(file.LoadNames)
					name=file.ReadString(size);
			}
			else if(gid==GID::SGuid) {
				guid.Load(data);
			}
			else if(gid==GID::Folder_Props) {
				reallyloadnames=file.ReadBool();

				file.EatChunk(size-4);
			}
			else if(!load) { // order dependent!
				file.EatChunk(size);
			}
			else if(gid==GID::Folder) {
				auto folder = new Folder(file);

				if(!folder->load(data, size, false, false, shallow)) {
					delete folder;

					return false;
				}

				children.Add(folder);

				if(onlyfirst) {
					data.seekg(target);
					break;
				}
			}
			else {
				auto resource = file.LoadObject(data, gid, size);

				if(resource) {
					children.Add(resource);
				}
#ifndef NDEBUG
				else {
					throw std::runtime_error("Cannot load resource: "+String::From(gid));
				}
#endif

				if(onlyfirst) {
					data.seekg(target);
					break;
				}
			}
		}

		return true;
	}

	Folder *LoadFolderResource(std::istream &data, unsigned long size, File &file, bool onlyfirst, bool shallow) {
		auto folder = new Folder(file);

		if(!folder->load(data, size, onlyfirst, shallow, true)) {
			delete folder;
			return nullptr;
		}

		return folder;
	}



} }
