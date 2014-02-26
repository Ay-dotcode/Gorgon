#pragma once

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <functional>

#include "GRE.h"
#include "../Utils/Collection.h"
#include "Definitions.h"
#include "Folder.h"

namespace gge { namespace resource {

	////This class defines a resource loader
	class Loader {
	public:
		////This is Resource loader function prototype
		typedef std::function<Base* (File&, std::istream&, int)> LoaderFunction;

		////Gorgon ID of the resource
		GID::Type GId;

		////Load handler function
		LoaderFunction Handler;

		////Filling constructor
		Loader(GID::Type gid, LoaderFunction handler) : GId(gid), Handler(handler) 
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
			Containment		= 4,
			UnknownNode		= 5 //never fired by gge loader
		};

		class strings {
		public:
			static const std::string FileNotFound	;
			static const std::string Signature		;
			static const std::string VersionMismatch;
			static const std::string Containment	;
			static const std::string UnknownNode	;
		};


		load_error(ErrorType number, const std::string &text) : runtime_error(text), number(number) {

		}

		load_error(ErrorType number, const char *text) : runtime_error(text), number(number) {

		}

		ErrorType number;
	};

	class File {
	public:
		File() : root(new Folder), LoadNames(false) {

		}

		////Resource Loaders
		utils::Collection<Loader>	Loaders;
		utils::Collection<Redirect>	Redirects;
		std::map<Base*, int>		Multiples;

		////File type
		GID::Type FileType;
		////File version
		int FileVersion;
		bool LoadNames;
		Folder &Root() { return *root; }
		////Returns the filename used for the last load or save operation
		std::string GetFilename() const { return Filename; }

		////Loads a given file throws load_error
		///@Filename	: File to be loaded
		void LoadFile(const std::string &Filename) { load(Filename, false); }

		void LoadFirst(const std::string &Filename) { load(Filename, true);}

		////Returns if a file is loaded
		bool isLoaded() const { return isloaded; }

		////Loads a resource object from the given file, GID and size
		Base *LoadObject(std::istream &Data, int GID, int Size);

		////Searches the given resource object within this file
		Base *FindObject(utils::SGuid guid);

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

		void Destroy() { utils::CheckAndDelete(root); }

		~File();

	protected:
		void load(const std::string &filename, bool first);

		////The root folder, root changes while loading a file
		Folder *root;

	private:
		bool isloaded;
		std::string Filename;
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
