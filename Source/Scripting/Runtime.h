/// @file Runtime.h This file contains classes that stores runtime and end programmer defined objects.

#pragma once

#include <ostream>
#include <string>


#include "Reflection.h"
#include "Data.h"
#include "Input.h"

namespace Gorgon {
	
	namespace Scripting {
		
		/// This class allows references to be counted and destroyed properly.
		class ReferenceCounter {
		public:
			
			/// Registers a new object of reference counting, this will set reference count to one. This function
			/// ignores register requests for nullptr
			void Register(const Data &data) {
				ASSERT(data.GetData().IsPointer(), "Reference keeping can only be performed for reference types, "
				"offender: "+data.GetType().GetName(), 1, 4);
				
				void *ptr=data.GetData().Pointer();
				
				//ignore register requests to nullptr
				if(ptr==nullptr) return;
				
				if(references[ptr]==0)
					references[ptr]=1;
			}
			
			/// Registers a new object of reference counting, this will set reference count to 0. This function
			/// ignores register requests for nullptr
			void Register(void *ptr) {
				//ignore register requests to nullptr
				if(ptr==nullptr) return;
				
				references[ptr];
			}
			
			/// Increases the reference count of the given object. If it is not registered, this request is ignored
			void Increase(const Data &data) {
				ASSERT(data.GetData().IsPointer(), "Reference keeping can only be performed for reference types, "
					   "offender: "+data.GetType().GetName(), 1, 4);

				void *ptr=data.GetData().Pointer();
				auto f=references.find(ptr);
				if(f==references.end()) return;

				f->second++;
			}
			
			/// Decreases the reference count of the given object. If it is not registered, this request is ignored.
			/// If reference count of the object reaches 0, it is deleted.
			void Decrease(const Data &data) {
				ASSERT(data.GetData().IsPointer(), "Reference keeping can only be performed for reference types, "
					   "offender: "+data.GetType().GetName(), 1, 4);

				void *ptr=data.GetData().Pointer();
				auto f=references.find(ptr);
				if(f==references.end()) return;

				int &v=f->second;
				v--;
				if(v<=0) {
					data.GetType().Delete(data);
					references.erase(f);
				}
			}
			
			/// Resets the reference count to 0
			void Reset(const Data &data) {
				ASSERT(data.GetData().IsPointer(), "Reference keeping can only be performed for reference types, "
					   "offender: "+data.GetType().GetName(), 1, 4);

				void *ptr=data.GetData().Pointer();
				auto f=references.find(ptr);
				if(f==references.end()) return;

				int &v=f->second;
				v=0;
			}
			
			bool IsRegistered(const Data &data) const {
				if(!data.IsValid()) return false;
				
				ASSERT(data.GetData().IsPointer(), "Reference keeping can only be performed for reference types, "
				"offender: "+data.GetType().GetName(), 1, 4);
				
				void *ptr=data.GetData().Pointer();
				auto f=references.find(ptr);
				if(f==references.end()) return false;
				
				return true;
			}
			
			/// Unregisters an object from reference counter
			void Unregister(const Data &data) {
				ASSERT(data.GetData().IsPointer(), "Reference keeping can only be performed for reference types, "
				"offender: "+data.GetType().GetName(), 1, 4);
				
				void *ptr=data.GetData().Pointer();
				references.erase(ptr);
			}
			
			/// Never use without a proper reason. Gets rid of the data without destroying its content. It does
			/// decrease reference count. However, if it hits 0 it will 
			void GetRidOf(Data &data) {
				ASSERT(data.GetData().IsPointer(), "Reference keeping can only be performed for reference types, "
				"offender: "+data.GetType().GetName(), 1, 4);
				
				void *ptr=data.GetData().Pointer();
				auto f=references.find(ptr);
				if(f==references.end()) return;
				
				int &v=f->second;
				v++;
				data=Data::Invalid();
				v--;
			}

		private:
			std::map<void*, int> references;
		};
		
		/// This class represents a variable. It contains the data and the name of the variable.
		class Variable : public Data {
		public:
			
			/// Creates an invalid variable
			Variable(const std::string &name="", const Data &data=Data::Invalid()) : name(name), Data(data) { }
			
			/// Constructor that sets the name, type and value of the variable. Unless this variable is
			/// declared inside an executing code, definedin should be left nullptr.
			Variable(const std::string &name, const Type &type, const Any &value) : 
			name(name), Data(type, value) {
			}
			
			
			/// Constructor that sets the name, and type of the variable. Default value of the specified
			/// type is used as value. Unless this variable is declared inside an executing code, definedin 
			/// should be left nullptr.
			Variable(const std::string &name, const Type &type) :
			name(name), Data(type) {
			}
			
			
			/// Sets the data contained in this variable without changing its type
			void Set(Any value) {
				data.Swap(value);
			}
			
			/// Sets the data contained in this variable
			void Set(const Data &value) {
				data=value.GetData();
				type=value.GetType();
			}
			
			/// Sets the data contained in this variable by modifying its type. Also this function
			/// resets the tags unless they are re-specified
			void Set(const Type &type, Any value) {
				data.Swap(value);
				this->type=&type;
			}

			/// Returns the name of the variable
			std::string GetName() const {
				return name;
			}

			
		private:
			std::string name;
		};
		
	}
}
