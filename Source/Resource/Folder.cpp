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

	bool Folder::Load(bool shallow) {
		if(fullyloaded) return true;

		auto f=this->file.lock();
		if(!f) {
			return false;
		}

		auto &file=*f;

		std::istream &data=file.open();

		file.Seek(entrypoint-4);

		auto size=file.ReadChunkSize();

		auto ret=load(data, size, false, shallow, true);

		if(ret) {
			//update mapping
			std::vector<Containers::Collection<Base>::ConstIterator> openlist;
			openlist.push_back(this->begin());

			while(openlist.size()) {
				if(!openlist.back().IsValid()) {
					openlist.pop_back();
					continue;
				}

				auto &obj=(*openlist.back());
				openlist.back().Next();
				if(obj.Children.GetCount()>0)
					openlist.push_back(obj.begin());

				file.mapping[obj.GetGuid()]=&obj;
			}

			Resolve(file);
		}

		return ret;
	}

	bool Folder::load(std::istream &data, unsigned long totalsize, bool onlyfirst, bool shallow, bool load) {
		unsigned long target = data.tellg()+totalsize;

		entrypoint = (unsigned long)data.tellg();

		auto f=this->file.lock();
		if(!f) {
			return false;
		}

		auto &file=*f;

		if(!load) { 
			file.KeepOpen();
		}

		while(data.tellg()<target) {
			auto gid = file.ReadGID();
			auto size= file.ReadChunkSize();

			if(gid==GID::Folder_Names) {
				file.EatChunk(size);
			}
			else if(gid==GID::Folder_Props) {
				reallyloadnames=file.ReadBool();

				file.EatChunk(size-4);
			}
			else if(load && gid==GID::Folder) {
				auto folder = new Folder(file);

				if(!folder->load(data, size, false, false, !shallow)) {
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
				auto resource = file.LoadChunk(*this, data, gid, size, !load);

				if(resource) {
					children.Add(resource);

					if(onlyfirst) {
						data.seekg(target);
						break;
					}
				}

			}
		}

		return true;
	}

	Folder *Folder::LoadResource(File &file, std::istream &data, unsigned long size) {
		auto folder = new Folder(file);

		if(!folder->load(data, size, false, false, true)) {
			delete folder;
			return nullptr;
		}

		return folder;
	}



} }
