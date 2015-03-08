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
			Data(const Type *type, const Any &data, bool isreference=false, bool isconstant=false);

			/// Any constructor. Allows both data and type to be specified
			Data(const Type &type, const Any &data, bool isreference=false, bool isconstant=false) : Data(&type, data, isreference, isconstant) {
			}
			
			/// Default value constructor. Value of the data is determined from the type
			Data(const Type &type);

			/// Assignment operator
			Data &operator =(Data);
			
			bool IsNull() const;
			
			/// Returns the value of this data in the requested format
			template <class T_>
			const typename std::remove_reference<T_>::type &GetValue() const {
				if(isreference)
					return *data.Get<typename std::remove_reference<T_>::type*>();
				else
					return data.Get<typename std::remove_reference<T_>::type>();
			}
			
			/// Returns the value of this data in the requested format
			template <class T_>
			T_ &GetValue() {
				if(isreference)
					return *data.Get<typename std::remove_reference<T_>::type*>();
				else
					return data.Get<typename std::remove_reference<T_>::type>();
			}
			
			
			/// Returns the value of this data in the requested format. Requested type should
			/// ideally be a reference. Though this is not a requirement.
			template <class T_>
			T_ ReferenceValue() const {
				ASSERT(isreference, "The data contained is not a reference");
				
				return *data.Get<typename std::remove_reference<T_>::type*>();
			}
			
			/// Returns the data contained in this data element
			Any GetData() const {
				return data;
			}
			
			Data GetReference();
			
			/// Returns if the data is in a valid state
			bool IsValid() const {
				return type != nullptr;
			}
			
			/// Returns if this data contains a reference
			bool IsReference() const {
				return isreference;
			}
			
			/// Returns if this data is constant
			bool IsConstant() const {
				return isconstant;
			}
			
			/// Makes this data a constant
			void MakeConstant() {
				isconstant=true;
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
			
			/// Is a reference, data is a ptr to the original type
			bool isreference = false;
			
			/// This data is a constant and should not be changed
			bool isconstant = false;
			
			
		private:
			/// Constructs an invalid data. Performing any operation on this data might cause
			/// crashes. Never use this constructor unless its absolutely necessary
			Data() {}
			
			void check();
		};
		
		
	}
}