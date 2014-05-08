#include "Base.h"


namespace Gorgon { namespace Resource {
	void Base::Resolve(File &file) {
		for(auto it=children.First(); it.IsValid(); ) {

			Base &r=*it;
			it.Next();

			r.Resolve(file);
		}
	}

	void Base::Prepare() {
		for(auto &child : *this) {
			child.Prepare();
		}
	}

	void Base::Discard() {
		for(auto &child : *this) {
			child.Discard();
		}
	}

	Base::Base() : Children(children) { }

	Base::~Base() {
		for(auto it=children.First(); it.IsValid(); ) {
			if( --it->refcount == 0 ) {
				children.Delete(*it);
			}
			else {
				// making sure that we don't leave any objects in
				// undetermined state
				if(it->parent==this) {
					it->parent=nullptr;
					it->root=nullptr;
				}
				
				it.Next();
			}
		}
	}

} }
