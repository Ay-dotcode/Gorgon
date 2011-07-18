#pragma once

#include <stdexcept>
#include <iostream>
#include <fstream>

#include "GRE.h"
#include "../Utils/Collection.h"
#include "Definitions.h"
#include "FolderResource.h"

namespace gge { namespace resource {
	////This is Resource loader function prototype
	typedef ResourceBase& (*ResourceLoaderFunction)(File& File, FILE& Data, int Size);

	////This class defines a resource loader
	class ResourceLoader {
	public:
		////Gorgon ID of the resource
		int GID;

		////Load handler function
		ResourceLoaderFunction Loader;

		////Filling constructor
		ResourceLoader(int gid, ResourceLoaderFunction loader) {
			GID=gid;
			Loader=loader;
		}
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
		////Resource Loaders
		utils::Collection<ResourceLoader> Loaders;
		utils::Collection<Redirect> Redirects;

		////File type
		GID::Type FileType;
		////File version
		int FileVersion;
		FolderResource &Root() { return *root; }
		////Returns the filename used for the last load or save operation
		string getFilename() const { return Filename; }

		////Loads a given file modifying error variables
		///@Filename	: File to be loaded
		bool LoadFile(const string &Filename);

		////Returns if a file is loaded
		bool isLoaded() const { return isloaded; }

		////Loads a resource object from the given file, GID and size
		ResourceBase &LoadObject(std::ifstream &Data, int GID, int Size);

		////Searches the given resource object within this file
		ResourceBase *FindObject(utils::SGuid guid) { 
			if(guid.isEmpty()) 
				return NULL;

			for(utils::Collection<Redirect>::Iterator i=Redirects.First();i.isValid();i.Next()) {
				if(i->source==guid)
					guid=i->target;
			}
			
			return root->FindObject(guid); 
		}

		////Adds basic resource loaders 
		void AddBasicLoaders();

		////Adds extended resource loaders
		void AddExtendedLoaders();

		////Adds game resource loaders
		void AddGameLoaders();

		void Prepare(GGEMain &main) { root->Prepare(main); }
		void Prepare(GGEMain *main) { root->Prepare(*main); }
		void Prepare() { root->Prepare(Main); }

	protected:
		////The root folder, root changes while loading a file
		FolderResource *root;

	private:
		bool isloaded;
		string Filename;
	};

	inline void EatChunk(FILE *Data, int Size) { fseek(Data, Size, SEEK_CUR); }
} }
