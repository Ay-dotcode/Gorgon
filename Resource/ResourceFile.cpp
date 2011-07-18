#include "ResourceFile.h"
#include "TextResource.h"
#include "ImageResource.h"
#include "DataResource.h"
#include "AnimationResource.h"
#include "BitmapFontResource.h"
#include "SoundResource.h"
#include "FontTheme.h"
#include "LinkNode.h"
#include "FontTheme.h"

namespace gge { namespace resource {

	bool File::LoadFile(const string &Filename) {
		char sgn[7];

		this->Filename=Filename;

		///*Check file existence
		FILE *data;
		errno_t err;
		err=fopen_s(&data, Filename.data(), "rb");
		if(data==NULL) {
			throw load_error(load_error::FileNotFound, load_error::strings::FileNotFound);
		}


		fread(sgn,6,1,data);
		sgn[6]=0;
		///*Check file signature
		if(strcmp(sgn,"GORGON")!=0) {
			fclose(data);
			throw load_error(load_error::Signature, load_error::strings::Signature);
		}

		///*Check file version
		fread(&FileVersion,1,4,data);
		if(FileVersion>CurrentVersion) {
			fclose(data);
			throw load_error(load_error::VersionMismatch, load_error::strings::VersionMismatch);
		}

		///*Load file type
		fread(&FileType,1,4,data);

		///*Check first element
		int tmpint;
		fread(&tmpint,1,4,data);
		if(tmpint!=GID::Folder) {
			fclose(data);
			throw load_error(load_error::Containment, load_error::strings::Containment);
		}

		int size;
		fread(&size,1,4,data);

		///*Load first element
		root=(FolderResource*)LoadFolderResource(this,data,size);
		root->Resolve();

		isloaded=1;

		///*Close file
		fclose(data);
		return true;
	}

	ResourceBase *File::LoadObject(FILE *Data, int GID, int Size) {
		ResourceLoader *loader;
		Loaders.ResetIteration();

		while(loader=Loaders.next()) {
			if(loader->GID==GID) {
				return loader->Loader(this, Data, Size);
			}
		}

		EatChunk(Data,Size);
		return NULL;
	}

	void File::AddBasicLoaders() {
		Loaders.Add(new ResourceLoader(GID_FOLDER, LoadFolderResource)); 
		Loaders.Add(new ResourceLoader(GID_LINKNODE, LoadLinkNodeResource)); 
		Loaders.Add(new ResourceLoader(GID_TEXT, LoadTextResource)); 
		Loaders.Add(new ResourceLoader(GID_IMAGE, LoadImageResource)); 
		Loaders.Add(new ResourceLoader(GID_DATAARRAY, LoadDataResource)); 
	}

	void File::AddExtendedLoaders() {
		AddBasicLoaders();
		Loaders.Add(new ResourceLoader(GID_SOUND, LoadSoundResource)); 
	}

	void File::AddGameLoaders() {
		AddExtendedLoaders();
		Loaders.Add(new ResourceLoader(GID_ANIMATION, LoadAnimationResource)); 
		Loaders.Add(new ResourceLoader(GID_FONT, LoadBitmapFontResource)); 
		Loaders.Add(new ResourceLoader(GID_FONTTHEME, LoadFontTheme)); 
		Loaders.Add(new ResourceLoader(GID_FONTTHEME, LoadFontTheme)); 
	}

	const string load_error::strings::FileNotFound		= "Cannot find the file specified";
	const string load_error::strings::Signature			= "Signature mismatch";
	const string load_error::strings::VersionMismatch	= "Version mismatch";
	const string load_error::strings::Containment		= "The supplied file is does not contain any data or its representation is invalid.";

} }
