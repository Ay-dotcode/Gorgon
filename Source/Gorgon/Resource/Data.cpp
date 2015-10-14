#include "Data.h"


#include "File.h"

namespace Gorgon { namespace Resource {
	
	void Data::save(Writer& writer) { 
		auto start=writer.WriteObjectStart(this);
		
		for(auto &item : items) {
			item.Save(writer);
		}
		
		writer.WriteEnd(start);
	}

} }
