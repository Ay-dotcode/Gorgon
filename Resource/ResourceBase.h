#pragma once

#include "Definitions.h"
#include "../Utils/Collection.h"
#include "GRE.h"
#include "../Engine/GGEMain.h"

using namespace gge;

namespace gre {

	////This class is the base for all Gorgon Resources
	class ResourceBase {
	public:
		////This function shall return Gorgon ID of this resource
		virtual int getGID()=0;
		////This function shall prepare this resource to be used
		/// after file is loaded, default behaviour is to pass
		/// the request to children
		virtual void Prepare(GGEMain *main);
		////This function shall resolve links or similar constructs
		virtual void Resolve();

		ResourceBase();
		virtual ~ResourceBase();
		
		////This function shall save this resource to the given file
		virtual bool Save(ResourceFile *File, FILE *Data)=0;

		////This function tests whether this object has the given Guid
		bool isEqual(Guid *guid) { if(guid==NULL) return false; return (*guid)==this->guid; }

		////Guid to identify this resource object
		Guid *guid;
		////Name of this resource object, may not be loaded.
		string name;
		////Caption of this resource object, may not be loaded.
		string caption;

		////Subitems that this resource object have. Some of the subitems
		/// can be hidden therefore, this is not guaranteed to be complete
		LinkedList<ResourceBase> Subitems;

		////Searches the public children of this resource object
		virtual ResourceBase *FindObject(Guid *guid);

		////Searches the public children of this resource object
		virtual ResourceBase *FindParent(Guid *guid);
	};
}
