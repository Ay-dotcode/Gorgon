#pragma once

#include "GRE.h"
#include "../Utils/Collection.h"
#include "Definitions.h"
#include "FolderResource.h"

namespace gre {
	////This is Resource loader function prototype
	typedef ResourceBase* (*ResourceLoaderFunction)(ResourceFile* File, FILE* Data, int Size);

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
		Redirect(Guid &source, Guid &target) : 
		  source(new Guid(source)), target(new Guid(target)) { }
		Guid *source;
		Guid *target;
	};

	class ResourceFile {
	public:
		////Resource Loaders
		Collection<ResourceLoader> Loaders;
		Collection<Redirect> Redirects;

		////File type
		int FileType;
		////File version
		int FileVersion;
		FolderResource &Root() { return *root; }
		////When loading or saving a file represents code of the error.
		/// If no error occurs has the value of 0.
		int ErrorNo;
		////When loading or saving a file represents error text.
		/// If no error occurs, this variable is set to NULL.
		char *ErrorText;
		////Returns the filename used for the last load or save operation
		string getFilename() { return Filename; }

		////Loads a given file modifying error variables
		///@Filename	: File to be loaded
		bool LoadFile(string Filename);

		////Returns if a file is loaded
		bool isLoaded() { return isloaded; }

		////Loads a resource object from the given file, GID and size
		ResourceBase *LoadObject(FILE *Data, int GID, int Size);

		////Searches the given resource object within this file
		ResourceBase *FindObject(Guid *guid) { 
			if(guid==NULL)
				return NULL;

			if(guid->isEmpty()) 
				return NULL;

			foreach(Redirect, redirect, Redirects) {
				if(redirect->source->isEqual(*guid))
					guid=redirect->target;
			}
			
			return root->FindObject(guid); 
		}

		////Adds basic resource loaders 
		void AddBasicLoaders();

		////Adds extended resource loaders
		void AddExtendedLoaders();

		////Adds game resource loaders
		void AddGameLoaders();

		void Prepare(GGEMain &main) { root->Prepare(&main); }
		void Prepare(GGEMain *main) { root->Prepare(main); }

	protected:
		////The root folder, root changes while loading a file
		FolderResource *root;

	private:
		bool isloaded;
		string Filename;
	};

	inline void EatChunk(FILE *Data, int Size) { fseek(Data, Size, SEEK_CUR); }
}