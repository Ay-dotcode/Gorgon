#include "../Filesystem.h"
#include "../Encoding/LZMA.h"

#include "File.h"
#include "Blob.h"
#include "Image.h"
#include "Animation.h"



namespace Gorgon { namespace Resource {

	File::File() : root(new Folder), LoadNames(false), self(this, [](File*) { }) { 
		Loaders[GID::Blob] ={GID::Blob, Blob::LoadResource};
		Loaders[GID::Folder] ={GID::Folder, Folder::LoadResource};
		Loaders[GID::Image] ={GID::Image, Image::LoadResource};
		Loaders[GID::Animation_Image] ={GID::Animation_Image, Image::LoadResource};
		Loaders[GID::Animation] ={GID::Animation, Animation::LoadResource};
	}

	Base *File::LoadChunk(Base &self, std::istream &data, GID::Type gid, unsigned long size, bool skipobjects) {
		if(gid==GID::SGuid) {
			self.guid.Load(data);
			return nullptr;
		}
		else if(gid==GID::Name) {
			self.name=ReadString(size);
			return nullptr;
		}
		else if(skipobjects) {
			EatChunk(size);
			return nullptr;
		}

		for(auto &loader : Loaders) {

			if(loader.second.GID==gid) {
				auto obj=loader.second.Handler(*this, data, size);
#ifndef NDEBUG
				if(!obj) throw std::runtime_error("Cannot load the object with GID"+String::From(gid));
#endif
				return obj;
			}
		}

#ifndef NDEBUG
		throw std::runtime_error("No handler for GID"+String::From(gid));
#endif

		EatChunk(size);

		return nullptr;
	}

	void File::load(const std::string &filename, bool first, bool shallow) {
		delete root;
		file=new std::ifstream;
		std::ifstream &data=*file;

		try {
			if(!Filesystem::IsFile(filename) && Filesystem::IsFile(filename+".lzma")) {
				std::ifstream ifile(filename+".lzma", std::ios::binary);
				std::ofstream ofile(filename, std::ios::binary);
				Encoding::Lzma.Decode(ifile, ofile);
				Filesystem::Delete(filename+".lzma");
			}

			if(!Filesystem::IsFile(filename)) {
				throw LoadError(LoadError::FileNotFound, LoadError::ErrorStrings[LoadError::FileNotFound]+"\n"+filename);
			}

			char sgn[7];

			this->filename=Filesystem::Canonical(filename);

			//Check file existence
			data.open(filename, std::ios::in | std::ios::binary);
			if(data.fail())
				throw LoadError(LoadError::FileCannotBeOpened, LoadError::ErrorStrings[LoadError::FileCannotBeOpened]+"\n"+filename);


			//Check file signature
			data.read(sgn, 6);
			sgn[6]=0;
			if(std::string("GORGON")!=sgn)
				throw LoadError(LoadError::Signature);

			//Check file version
			fileversion=ReadInt32();
			if(fileversion>CurrentVersion)
				throw LoadError(LoadError::VersionMismatch);

			//Load file type
			filetype=ReadGID();

			//Check first element
			if(ReadGID()!=GID::Folder)
				throw LoadError(LoadError::Containment);

			unsigned long size=ReadUInt32();

			//Load first element
			root=new Folder(*this);
			root->load(data, size, first, shallow, true);

			if(!root)
				throw LoadError(LoadError::Containment);
		}
		catch(...) {
			root=new Folder;
			delete file;
			file=nullptr;

			throw;
		}

		isloaded=true;

		//Close file
		if(!keepopen) {
			data.close();
			delete file;
			file=nullptr;
		}

		//build mapping
		std::vector<Containers::Collection<Base>::ConstIterator> openlist;
		openlist.push_back(root->begin());
		mapping[root->GetGuid()]=root;

		while(openlist.size()) {
			if(!openlist.back().IsValid()) {
				openlist.pop_back();
				continue;
			}

			auto &obj=(*openlist.back());
			openlist.back().Next();
			if(obj.Children.GetCount()>0)
				openlist.push_back(obj.begin());

			mapping[obj.GetGuid()]=&obj;
		}

		root->Resolve(*this);
	}

	const std::string LoadError::ErrorStrings[7] ={
		"Unknown error", 
		"Cannot find the file specified", 
		"Signature mismatch",
		"Version mismatch", 
		"The supplied file is does not contain any data or its representation is invalid.",
		"An unknown node is encountered in the file.",
		"Cannot open the file specified."
	};

} }
