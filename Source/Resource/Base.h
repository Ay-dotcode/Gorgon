#pragma once

#include "../Containers/Collection.h"
#include "../SGuid.h"

#include "GID.h"

namespace Gorgon {
	class Window;
	
	/// This namespace contains all Gorgon resources. 
	namespace Resource {

		class File;

		/// This class is the base for all Gorgon Resources. 
		/// @warning This class is rather heavy and should not be used for small objects that are
		/// planned to be created a lot of.
		class Base {
			friend class File;
		public:

			/// Default constructor
			Base();
			
			/// Destructor, Always children gets destroyed first
			virtual ~Base();


			/// This function shall return Gorgon ID of this resource
			virtual GID::Type GetGID() const = 0;


			/// This function shall resolve links or similar constructs. This function is intended to be called
			/// after a file is loaded. It has no meaning for in memory constructed resource trees. Default
			/// behavior is to pass the request to children.
			virtual void Resolve(File &file);

			/// This function shall prepare this resource to be used
			/// after resource is loaded. Default behavior is to pass
			/// the request to children
			virtual void Prepare();

			/// This function shall discard any transitional data which is not vital
			/// after Prepare function is issued. This data can be image pixel buffer
			/// sound data buffer. Default behavior is to pass the request to children
			virtual void Discard();


			
			/// This function tests whether this object has the given SGuid
			virtual bool IsEqual(const SGuid &guid) const { return guid==this->guid; }
			
			/// Returns the guid of the object
			virtual SGuid GetGuid() const { return guid; }
			
			
			/// Returns the name of this object.
			/// @warning The object names are loaded only upon request
			const std::string &GetName() const { return name; }
			
			
			/// Returns whether this object has a parent
			bool HasParent() const { return parent!=nullptr; }
			
			/// Returns the parent. If no parent set, this function throws std::runtime_error
			Base &GetParent() const { 
				if(!parent) 
					throw std::runtime_error("This object has no parent");
				
				return *parent; 
			}
			
			/// Returns the pointer to the parent. This function may return nullptr
			Base *GetParentPtr() const {
				return parent;
			}
						
			/// Returns the root of this resource. Root of a resource is always exists, in case
			/// of no parent, the root is the object itself. Note that this value is cached and maintained.
			/// In rare cases, it will be recalculated.
			const Base &GetRoot() const {
				if(!root) {
					if(!parent) {
						root=this;
					}
					else {
						root=parent->root;
					}
				}
				
				return *root;
			}

			
			/// Allows easy iteration through range based fors
			const Containers::Collection<Base>::ConstIterator begin() const {
				return Children.begin();
			}
			
			/// Allows easy iteration through range based fors
			const Containers::Collection<Base>::ConstIterator end() const {
				return Children.end();
			}

			
			/// The children this object have. The elements in const collections are modifiable,
			/// therefore, its possible to modify properties of the children. However, children
			/// should be added to the object using member methods as some objects do not allow
			/// children, or allow children that are of specific type.
			const Containers::Collection<Base> &Children;

			/// **INTERNAL**, Reference count, used in linking mechanism.
			/// @warning Never change or rely on this value unless you know the internal mechanics
			///          of linking system. Any uninformed changes may cause leaks or worse, double
			///          deletion and crash of the program.
			unsigned long refcount=1;

		protected:

			/// Destroys the children of this resource
			void destroychildren();

			/// Sets the parent of an object to nullptr, provides access.
			void setparenttonullptr(Base &base) { base.parent=nullptr; base.root=nullptr; }

			/// SGuid to identify this resource object
			SGuid guid;
			
			/// Name of this resource object, may not be loaded.
			std::string name;
			
			/// Immediate parent of this resource
			Base *parent=nullptr;

			/// Child objects that this resource object have. Some of child objects
			/// can be private, therefore, this is not guaranteed to be complete
			Containers::Collection<Base> children;
			
			/// Root of this resource
			mutable const Base *root=nullptr;
		};
	
	} 
	
}
