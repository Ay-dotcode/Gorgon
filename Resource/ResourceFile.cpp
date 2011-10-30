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

using namespace std;
using namespace gge::utils;

namespace gge { namespace resource {

	void File::LoadFile(const string &Filename) {
		char sgn[7];

		this->Filename=Filename;

		///*Check file existence

		ifstream data;
		data.open(Filename, ios::in | ios::binary);
		if(data.fail())
			throw load_error(load_error::FileNotFound, load_error::strings::FileNotFound);


		///*Check file signature
		data.read(sgn, 6);
		sgn[6]=0;
		if(string("GORGON")!=sgn)
			throw load_error(load_error::Signature, load_error::strings::Signature);

		///*Check file version
		ReadFrom(data, FileVersion);
		if(FileVersion>CurrentVersion)
			throw load_error(load_error::VersionMismatch, load_error::strings::VersionMismatch);

		///*Load file type
		ReadFrom(data, FileType);

		///*Check first element
		if(ReadFrom<int>(data)!=GID::Folder)
			throw load_error(load_error::Containment, load_error::strings::Containment);

		int size;
		ReadFrom(data, size);

		///*Load first element
		root=dynamic_cast<FolderResource*>(LoadFolderResource(*this, data, size));
		if(!root)
			throw load_error(load_error::Containment, load_error::strings::Containment);

		root->Resolve(*this);

		isloaded=true;

		///*Close file
		data.close();
	}

	ResourceBase *File::LoadObject(istream &Data, int GID, int Size) {
		for(utils::Collection<ResourceLoader>::Iterator loader=Loaders.First();
			loader.isValid(); loader.Next()) {

			if(loader->GId==GID) {
				return loader->Loader(*this, Data, Size);
			}
		}

		EatChunk(Data,Size);
		return NULL;
	}

	void File::AddBasicLoaders() {
		Loaders.Add(new ResourceLoader(GID::Folder, std::bind(LoadFolderResource, placeholders::_1, placeholders::_2, placeholders::_3, false)));
		Loaders.Add(new ResourceLoader(GID::LinkNode, LoadLinkNodeResource)); 
		Loaders.Add(new ResourceLoader(GID::Text, LoadTextResource)); 
		Loaders.Add(new ResourceLoader(GID::Image, LoadImageResource)); 
		Loaders.Add(new ResourceLoader(GID::Data, LoadDataResource)); 
	}

	void File::AddExtendedLoaders() {
		AddBasicLoaders();
		Loaders.Add(new ResourceLoader(GID::Sound, LoadSoundResource)); 
	}

	void File::AddGameLoaders() {
		AddExtendedLoaders();
		Loaders.Add(new ResourceLoader(GID::Animation, LoadAnimationResource)); 
		Loaders.Add(new ResourceLoader(GID::Font, LoadBitmapFontResource)); 
		Loaders.Add(new ResourceLoader(GID::FontTheme, LoadFontTheme)); 
		Loaders.Add(new ResourceLoader(GID::FontTheme, LoadFontTheme)); 
	}

	ResourceBase * File::FindObject( utils::SGuid guid ) {
		if(guid.isEmpty()) 
			return NULL;

		for(utils::Collection<Redirect>::Iterator i=Redirects.First();i.isValid();i.Next()) {
			if(i->source==guid)
				guid=i->target;
		}

		return root->FindObject(guid);
	}

	const string load_error::strings::FileNotFound		= "Cannot find the file specified";
	const string load_error::strings::Signature			= "Signature mismatch";
	const string load_error::strings::VersionMismatch	= "Version mismatch";
	const string load_error::strings::Containment		= "The supplied file is does not contain any data or its representation is invalid.";

} }
