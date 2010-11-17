#pragma once

#include "GRE.h"
#include "ResourceBase.h"
#include "TextResource.h"
#include "ImageResource.h"
#include "DataResource.h"
#include "SoundResource.h"
#include "AnimationResource.h"
#include "BitmapFontResource.h"

namespace gre {
	class ResourceFile;
	
	////This function loads a folder resource from the given file
	ResourceBase *LoadFolderResource(ResourceFile* File, FILE* Data, int Size);

	////This is basic folder resource, it holds other resources.
	class FolderResource : public ResourceBase {
		friend ResourceBase *LoadFolderResource(ResourceFile* File, FILE* Data, int Size);
	public:
		////Entry point of this resource within the physical file. If
		/// no file is associated with this resource this value is -1.
		int EntryPoint;

		////01010000h, (System, Folder)
		virtual int getGID() { return GID_FOLDER; }
		////Currently does nothing
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }

		////Returns the number of items contained
		int			 getCount() { return Subitems.getCount(); }
		////Returns an item with the given index
		ResourceBase	*getItem (int Index) { return Subitems[Index]; }
		////Returns an item with the given index
		ResourceBase	&operator [] (int Index) { return *(Subitems[Index]); }
		////Adds a new resource to this folder
		void	Add(ResourceBase *resource) { Subitems.AddItem(resource); }
		////Adds a new resource to this folder
		FolderResource	&operator << (ResourceBase &resource) { Subitems.AddItem(&resource); return *this; }

		////Returns the given subitem with folder resource type. Used to avoid type casting
		FolderResource	*asFolder	(int Index) { 
#ifdef _DEBUG
			if(Subitems[Index]->getGID()!=GID_FOLDER) {
				DisplayMessage("Folder Resource","Non folder item requested as folder!");
				assert(0);
			}
#endif
			return (FolderResource*)Subitems[Index]; 
		}
		TextResource	*asText		(int Index) { 
#ifdef _DEBUG
			if(Subitems[Index]->getGID()!=GID_TEXT) {
				DisplayMessage("Folder Resource","Non text item requested as text!");
				assert(0);
			}
#endif
			return (TextResource*)Subitems[Index]; 
		}
		ImageResource	*asImage	(int Index) { 
#ifdef _DEBUG
			if(Subitems[Index]->getGID()!=GID_IMAGE) {
				DisplayMessage("Folder Resource","Non image item requested as image!");
				assert(0);
			}
#endif
			return (ImageResource*)Subitems[Index]; 
		}
		DataResource	*asData		(int Index) { 
#ifdef _DEBUG
			if(Subitems[Index]->getGID()!=GID_DATAARRAY) {
				DisplayMessage("Folder Resource","Non data item requested as data!");
				assert(0);
			}
#endif
			return (DataResource*)Subitems[Index]; 
		}
		SoundResource	*asSound	(int Index) { 
#ifdef _DEBUG
			if(Subitems[Index]->getGID()!=GID_SOUND) {
				DisplayMessage("Folder Resource","Non sound item requested as sound!");
				assert(0);
			}
#endif
			return (SoundResource*)Subitems[Index]; 
		}
		AnimationResource	*asAnimation(int Index) { 
#ifdef _DEBUG
			if(Subitems[Index]->getGID()!=GID_ANIMATION) {
				DisplayMessage("Folder Resource","Non animation item requested as animation!");
				assert(0);
			}
#endif
			return (AnimationResource*)Subitems[Index]; 
		}
		BitmapFontResource	*asBitmapFont		(int Index) { 
#ifdef _DEBUG
			if(Subitems[Index]->getGID()!=GID_FONT) {
				DisplayMessage("Folder Resource","Non bitmap font item requested as bitmap font!");
				assert(0);
			}
#endif
			return (BitmapFontResource*)Subitems[Index]; 
		}
		
		FolderResource() : ResourceBase() {
			EntryPoint=-1;
		}
	};
}