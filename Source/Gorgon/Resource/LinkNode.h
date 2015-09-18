#pragma once


#include "Base.h"


namespace gge { namespace resource {
	class LinkNodeResource;
	
	LinkNodeResource *LoadLinkNodeResource(File &File, std::istream &Data, int Size);
	
	class LinkNodeResource : public Base {
		friend LinkNodeResource *LoadLinkNodeResource(File &File, std::istream &Data, int Size);
	public:

		LinkNodeResource() : target(nullptr) {  }

		////02020000h (Basic, Image)
		virtual GID::Type GetGID() const { return GID::LinkNode; }
		////Currently does nothing
		virtual bool Save(File &File, std::ostream &Data) { return false; }

		virtual void Resolve(File &file);

		Base *GetTarget(resource::File &file);

	protected:
		utils::SGuid target;
	};

} }
