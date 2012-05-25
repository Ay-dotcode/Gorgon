#pragma once

#include "../Utils/Collection.h"
#include "../Engine/GGEMain.h"
#include "../Utils/SGuid.h"

#include "GRE.h"
#include "Definitions.h"

#include <fstream>
#include <iosfwd>

namespace gge { namespace resource {


	////This class is the base for all Gorgon Resources
	class Base {
	public:
		////This function shall return Gorgon ID of this resource
		virtual GID::Type GetGID() const = 0;
		////This function shall prepare this resource to be used
		/// after file is loaded, default behavior is to pass
		/// the request to children
		virtual void Prepare(GGEMain &main, File &file);
		////This function shall resolve links or similar constructs
		virtual void Resolve(File &file);

		Base();
		virtual ~Base();
		
		////This function shall save this resource to the given file
		virtual bool Save(File &File, std::ostream &Data) { return false; }

		////This function tests whether this object has the given utils::SGuid
		bool isEqual(const utils::SGuid &guid) const { return guid==this->guid; }

		File *file;

		////utils::SGuid to identify this resource object
		utils::SGuid guid;
		////Name of this resource object, may not be loaded.
		std::string name;
		////Caption of this resource object, may not be loaded.
		std::string caption;

		////Subitems that this resource object have. Some of the sub items
		/// can be hidden therefore, this is not guaranteed to be complete
		utils::SortedCollection<Base> Subitems;

		////Searches the public children of this resource object
		virtual Base *FindObject(utils::SGuid guid);

		////Searches the public children of this resource object
		virtual Base *FindParent(utils::SGuid guid);
	};
} }
