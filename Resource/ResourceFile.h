#pragma once

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <functional>

#include "GRE.h"
#include "../Utils/Collection.h"
#include "Definitions.h"
#include "FolderResource.h"

namespace gge { namespace resource {

	////This class defines a resource loader
	class ResourceLoader {
	public:
		////This is Resource loader function prototype
		typedef std::function<ResourceBase* (File&, std::istream&, int)> LoaderFunction;

		////Gorgon ID of the resource
		GID::Type GId;

		////Load handler function
		LoaderFunction Loader;

		////Filling constructor
		ResourceLoader(GID::Type gid, LoaderFunction loader) : GId(gid), Loader(loader) 
		{ }
	};

	class Redirect {
	public:
		Redirect(utils::SGuid &source, utils::SGuid &target) : 
		  source(source), target(target) { }

		utils::SGuid source;
		utils::SGuid target;
	};

	class load_error : public std::runtime_error {
	public:

		enum ErrorType {
			FileNotFound	= 1,
			Signature		= 2,
			VersionMismatch	= 3,
			Containment		= 4
		};

		class strings {
		public:
			static const string FileNotFound	;
			static const string Signature		;
			static const string VersionMismatch	;
			static const string Containment		;
		};


		load_error(ErrorType number, const string &text) : runtime_error(text), number(number) {

		}

		load_error(ErrorType number, const char *text) : runtime_error(text), number(number) {

		}

		ErrorType number;
	};

	class File {
	public:
		File() : root(new FolderResource), LoadNames(false) {

		}

		////Resource Loaders
		utils::Collection<ResourceLoader> Loaders;
		utils::Collection<Redirect>		  Redirects;

		////File type
		GID::Type FileType;
		////File version
		int FileVersion;
		bool LoadNames;
		FolderResource &Root() { return *root; }
		////Returns the filename used for the last load or save operation
		string getFilename() const { return Filename; }

		////Loads a given file throws load_error
		///@Filename	: File to be loaded
		void LoadFile(const string &Filename);

		////Returns if a file is loaded
		bool isLoaded() const { return isloaded; }

		////Loads a resource object from the given file, GID and size
		ResourceBase *LoadObject(std::istream &Data, int GID, int Size);

		////Searches the given resource object within this file
		ResourceBase *FindObject(utils::SGuid guid);

		template<class T_>
		void FindObject(utils::SGuid guid, T_ *&object) {
			object=dynamic_cast<T_*>(FindObject(guid));
		}
		
		////Adds basic resource loaders 
		void AddBasicLoaders();

		////Adds extended resource loaders
		void AddExtendedLoaders();

		////Adds game resource loaders
		void AddGameLoaders();

		void Prepare(GGEMain &main) { root->Prepare(main, *this); }
		void Prepare(GGEMain *main) { root->Prepare(*main, *this); }
		void Prepare() { root->Prepare(Main, *this); }

	protected:
		////The root folder, root changes while loading a file
		FolderResource *root;

	private:
		bool isloaded;
		string Filename;
	};

	inline void EatChunk(std::istream &file, std::streamoff Size) { file.seekg(Size, std::ios::cur); }

	template<int B_, class T_>
	inline void ReadFrom(std::istream &Data, T_ &object) {
		Data.read(reinterpret_cast<char*>(&object), B_);
	}

	template<class T_>
	inline void ReadFrom(std::istream &Data, T_ &object) {
		Data.read(reinterpret_cast<char*>(&object), sizeof(object));
	}

	template<>
	inline void ReadFrom<std::string>(std::istream &Data, std::string &object) {
		int temp;
		ReadFrom(Data, temp);
		char *text=new char[temp];
		Data.read(text, temp);
		object.assign(text,temp);
	}

	template<class T_>
	inline T_ ReadFrom(std::istream &Data) {
		T_ object;
		Data.read(reinterpret_cast<char*>(&object), sizeof(object));

		return object;
	}
} }

inline int operator +(const std::streampos &l, int r) {
	return (int)l + r;
}
