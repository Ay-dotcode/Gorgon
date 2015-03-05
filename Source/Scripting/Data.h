#pragma once

#include "../Any.h"
#include "../Utils/Assert.h"

namespace Gorgon {
	
	namespace Scripting {
		
		class Type;
		
		/** 
		 * Data describes a piece of data. It contains boxed data and the type. Additionally,
		 * a data can be an array, or reference to a variable. Data can have two tags: ArrayTag
		 * and ReferenceTag. It is possible to use both tags together to create an array of
		 * references. When a data is a reference, its type indicates which type it refers to.
		 * However, the type it refers to could also be an array. This allows two dimensional
		 * arrays to exist. However, reference to reference is not valid for now. Data is 
		 * non-mutable after construction.
		 */
		class Data {
		public:
			
			/// Constructs an invalid data object. Data object does not perform validity check
			/// therefore, use of this function should be very limited.
			static Data Invalid() { return {}; }

			/// Copy constructor
			Data(const Data &other);

			/// Move constructor
			Data(Data &&other);

			/// Any constructor. Allows both data and type to be specified
			Data(const Type *type, const Any &data, bool isreference=false);

			/// Any constructor. Allows both data and type to be specified
			Data(const Type &type, const Any &data, bool isreference=false) : Data(&type, data, isreference) {
			}
			
			/// Default value constructor. Value of the data is determined from the type
			Data(const Type &type);

			/// Assignment operator
			Data &operator =(Data);
			
			bool IsNull() const;
			
			/// Returns the value of this data in the requested format
			template <class T_>
			const T_ &GetValue() const {
				if(isreference)
					return *data.Get<T_*>();
				else
					return data.Get<T_>();
			}
			
			/// Returns the value of this data in the requested format
			template <class T_>
			T_ &GetValue() {
				if(isreference)
					return *data.Get<T_*>();
				else
					return data.Get<T_>();
			}
			
			/// Returns the data contained in this data element
			Any GetData() const {
				return data;
			}
			
			/// Returns if the data is in a valid state
			bool IsValid() const {
				return type != nullptr;
			}
			
			/// Returns the type of the data
			const Type &GetType() const {
				ASSERT(type, "Type is not set", 1, 2);
				
				return *type;
			}
			
			std::string ToString() const;

			virtual ~Data();
			
		protected:
			/// Stored data
			Any   data;
			
			///Type of the data
			const Type *type = nullptr;
			
			bool isreference = false;
			
			
		private:
			/// Constructs an invalid data. Performing any operation on this data might cause
			/// crashes. Never use this constructor unless its absolutely necessary
			Data() {}
			
			void check();
		};
		
		
	}
}