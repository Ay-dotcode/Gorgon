#pragma once

#include "GRE.h"
#include "ResourceBase.h"

namespace gre {
	class ResourceFile;
	class FolderResource;
	class TextResource;
	class ImageResource;
	class DataResource;
	class SoundResource;
	class AnimationResource;
	class BitmapFontResource;
	
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
		FolderResource	*asFolder	(int Index);
		TextResource	*asText		(int Index);
		ImageResource	*asImage	(int Index);
		DataResource	*asData		(int Index);
		SoundResource	*asSound	(int Index);
		AnimationResource	*asAnimation(int Index);
		BitmapFontResource	*asBitmapFont		(int Index);

		template <typename T_>
		T_ &Get(int Index) {
			return dynamic_cast<T_&>(*(Subitems[Index]));
		}
		
		FolderResource() : ResourceBase() {
			EntryPoint=-1;
		}
	};
}