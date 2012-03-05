#pragma once

#include "GRE.h"
#include "Base.h"
#include <map>

namespace gge { namespace resource {
	class File;
	class Folder;
	class Text;
	class Image;
	class DataArray;
	class Sound;
	class Animation;
	class BitmapFont;
	
	////This function loads a folder resource from the given file
	Folder *LoadFolderResource(File &File, std::istream &Data, int Size, bool LoadNames=false);

	////This is basic folder resource, it holds other resources.
	class Folder : public Base {
		friend Folder *LoadFolderResource(File &File, std::istream &Data, int Size, bool LoadNames);
	public:
		Folder() : Base(), reallyloadnames(false), EntryPoint(-1) 
		{ }

		////Entry point of this resource within the physical file. If
		/// no file is associated with this resource this value is -1.
		int EntryPoint;

		////01010000h, (System, Folder)
		virtual GID::Type getGID() const { return GID::Folder; }
		////Currently does nothing
		virtual bool Save(File &File, std::ostream &Data) { return false; }

		////Returns the number of items contained
		int			 getCount() const { return Subitems.getCount(); }
		////Returns an item with the given index
		Base	*getItem (int Index) { return &Subitems[Index]; }
		////Returns an item with the given index
		Base	&operator [] (int Index) { return (Subitems[Index]); }
		////Adds a new resource to this folder
		void	Add(Base *resource) { Subitems.Add(resource, Subitems.HighestOrder()+1); }
		////Adds a new resource to this folder
		Folder	&operator << (Base &resource) { Subitems.Add(resource); return *this; }

		////Returns the given subitem with folder resource type. Used to avoid type casting
		Folder	*asFolder	(int Index);
		Text	*asText		(int Index);
		Image	*asImage	(int Index);
		DataArray	*asData		(int Index);
		Sound	*asSound	(int Index);
		Animation	*asAnimation(int Index);
		BitmapFont	*asBitmapFont		(int Index);

		//if you run into problems with dynamic_cast use CGet instead
		template <typename T_>
		T_ &Get(int Index) {
			return dynamic_cast<T_&>(Subitems[Index]);
		}

		//if you run into problems with dynamic_cast use CGetPtr instead
		template <typename T_>
		T_ *GetPtr(int Index) {
			return dynamic_cast<T_*>(&Subitems[Index]);
		}

		//try to use Get instead of this
		template <typename T_>
		T_ &CGet(int Index) {
			return (T_&)(Subitems[Index]);
		}

		//try to use GetPtr instead of this
		template <typename T_>
		T_ *CGetPtr(int Index) {
			return (T_*)(&Subitems[Index]);
		}

		bool Exists(int Index) const {
			return Index>=0 && Index<Subitems.getCount();
		}

		bool Exists(const std::string &Index) const {
			return namedlist.count(Index)>0;
		}

		//if you run into problems with dynamic_cast use CGet instead
		template <typename T_>
		T_ &Get(const std::string &Index) {
			if(namedlist.count(Index)>0 && dynamic_cast<T_*>(namedlist[Index]))
				return *dynamic_cast<T_*>(namedlist[Index]);
			else
				throw std::runtime_error("Requested item cannot be found");
		}

		//if you run into problems with dynamic_cast use CGetPtr instead
		template <typename T_>
		T_ *GetPtr(const std::string & Index) {
			return dynamic_cast<T_*>(namedlist[Index]);
		}

		//try to use Get instead of this
		template <typename T_>
		T_ &CGet(const std::string & Index) {
			if(namedlist.count(Index)>0)
				return *(T_*)(namedlist[Index]);
			else
				throw std::runtime_error("Requested item cannot be found");
		}

		//try to use GetPtr instead of this
		template <typename T_>
		T_ *CGetPtr(const std::string & Index) {
			return (T_*)(namedlist[Index]);
		}

		virtual void Prepare(GGEMain &main, File &file);

		virtual ~Folder() {

		}

	protected:
		bool reallyloadnames;
		std::map<std::string, Base*> namedlist;
	};
} }
