#include "File.h"
#include "../Filesystem.h"


namespace Gorgon { namespace Resource {

	Base *File::LoadObject(std::istream &data, GID::Type gid, unsigned long size) {
		for(auto &loader : Loaders) {

			if(loader.second.GID==gid) {
				auto obj=loader.second.Handler(data, size, this);
#ifndef NDEBUG
				if(!obj) throw std::runtime_error("Cannot load the object with GID"+String::From(gid));
#endif
				return obj;
			}
		}

#ifndef NDEBUG
		throw std::runtime_error("No handler for GID"+String::From(gid));
#endif

		EatChunk(data, size);

		return nullptr;
	}

	void File::load(const std::string &filename, bool first, bool shallow) {
		delete root;
		std::ifstream data;

		try {
			if(!Filesystem::IsFile(filename) && Filesystem::IsFile(filename+".lzma")) {
				std::ifstream ifile(filename+".lzma", std::ios::binary);
				std::ofstream ofile(filename, std::ios::binary);
				//Encoding::Lzma.Decode(ifile, ofile);
				Filesystem::Delete(filename+".lzma");
			}

			char sgn[7];

			this->filename=filename;

			//Check file existence
			data.open(filename, std::ios::in | std::ios::binary);
			if(data.fail())
				throw LoadError(LoadError::FileNotFound, LoadError::ErrorStrings[LoadError::FileNotFound]+"\n"+filename);


			//Check file signature
			data.read(sgn, 6);
			sgn[6]=0;
			if(std::string("GORGON")!=sgn)
				throw LoadError(LoadError::Signature);

			//Check file version
			fileversion=ReadInt32(data);
			if(fileversion>CurrentVersion)
				throw LoadError(LoadError::VersionMismatch);

			//Load file type
			filetype=ReadGID(data);

			//Check first element
			if(ReadGID(data)!=GID::Folder)
				throw LoadError(LoadError::Containment);

			unsigned long size=ReadUInt32(data);

			//Load first element
			root=LoadFolderResource(*this, data, size, LoadNames, first);
			if(!root)
				throw LoadError(LoadError::Containment);
		}
		catch(...) {
			root=new Folder;
			throw;
		}

		root->Resolve(*this);

		isloaded=true;

		//Close file
		data.close();
	}

	const std::string LoadError::ErrorStrings[6] ={
		"Unknown error", 
		"Cannot find the file specified", 
		"Signature mismatch",
		"Version mismatch", 
		"The supplied file is does not contain any data or its representation is invalid.",
		"An unknown node is encountered in the file."
	};

} }
