#include "Base.h"


namespace Gorgon { namespace Resource {
	void Base::Prepare() {
		for(auto &child : *this) {
			child.Prepare();
		}
	}

	void Base::Resolve() {
		for(auto it=children.First(); it.IsValid(); ) {

			Base &r=*it;
			it.Next();

			r.Resolve();
		}
	}

	Base *Base::FindObject(const SGuid &guid) const {
		for(auto &child : *this) {

			if(child.IsEqual(guid))
				return &child;

			Base *temp=child.FindObject(guid);
			
			if(temp)
				return temp;
		}

		return NULL;
	}

	Base::Base() { }

	Base::~Base() {
		for(auto it=children.First(); it.IsValid(); ) {
			if( --it->refcount == 0 ) {
				children.Delete(*it);
			}
			else {
				// making sure that we dont leave any objects in
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
