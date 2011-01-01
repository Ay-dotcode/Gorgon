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

namespace gre {

	bool ResourceFile::LoadFile(string filename) {
		char sgn[7];

		ErrorText=NULL;
		ErrorNo=0;
		Filename=filename;

		///*Check file existance
		FILE *data=fopen(filename.data(),"rb");
		if(data==NULL) {
			ErrorText=ERT_FILENOTFOUND;
			ErrorNo  =ERR_FILENOTFOUND;
			return false;
		}


		fread(sgn,6,1,data);
		sgn[6]=0;
		///*Check file signiture
		if(strcmp(sgn,"GORGON")!=0) {
			ErrorText=ERT_SIGNITURE;
			ErrorNo  =ERR_SIGNITURE;
			fclose(data);
			return false;
		}

		///*Check file version
		fread(&FileVersion,1,4,data);
		if(FileVersion>CURVERSION) {
			ErrorText=ERT_VERSION;
			ErrorNo  =ERR_SIGNITURE;
			fclose(data);
			return false;
		}

		///*Load file type
		fread(&FileType,1,4,data);

		///*Check first element
		int tmpint;
		fread(&tmpint,1,4,data);
		if(tmpint!=GID_FOLDER) {
			ErrorText=ERT_CONTAINMENT;
			ErrorNo  =ERR_CONTAINMENT;
			fclose(data);
			return false;
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

	ResourceBase *ResourceFile::LoadObject(FILE *Data, int GID, int Size) {
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

	void ResourceFile::AddBasicLoaders() {
		Loaders.Add(new ResourceLoader(GID_FOLDER, LoadFolderResource)); 
		Loaders.Add(new ResourceLoader(GID_LINKNODE, LoadLinkNodeResource)); 
		Loaders.Add(new ResourceLoader(GID_TEXT, LoadTextResource)); 
		Loaders.Add(new ResourceLoader(GID_IMAGE, LoadImageResource)); 
		Loaders.Add(new ResourceLoader(GID_DATAARRAY, LoadDataResource)); 
	}

	void ResourceFile::AddExtendedLoaders() {
		AddBasicLoaders();
		Loaders.Add(new ResourceLoader(GID_SOUND, LoadSoundResource)); 
	}

	void ResourceFile::AddGameLoaders() {
		AddExtendedLoaders();
		Loaders.Add(new ResourceLoader(GID_ANIMATION, LoadAnimationResource)); 
		Loaders.Add(new ResourceLoader(GID_FONT, LoadBitmapFontResource)); 
		Loaders.Add(new ResourceLoader(GID_FONTTHEME, LoadFontTheme)); 
		Loaders.Add(new ResourceLoader(GID_FONTTHEME, LoadFontTheme)); 
	}
}