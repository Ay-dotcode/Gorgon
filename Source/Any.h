/// @file Any.h contains class Any. Any is a container for any type and
///	supports boxing, unboxing and copying; uses RTTI. Best be used with
/// built in types or POD structures


#pragma once

#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <typeinfo>
#include <algorithm>



namespace Gorgon {

	/// This class can hold any other information providing type erasure. 
	/// It requires the type to be either copy constructible or
	/// move constructible. Type safety is ensured during debug
	/// using RTTI information.
	/// 
	/// This class only checks type safety in
	/// debug mode. In release it only checks for Any being empty. This check
	/// can also be removed if it causes bottleneck. Define GORGON_FAST_ANY
	/// to remove these checks.
	/// 
	class Any {
		/// Used internally by Any to unify different types.
		/// It is implemented by type dependent Type class.
		class TypeInterface {
		public:
			virtual void *New() const=0;
			virtual void *New() const=0;
			virtual void  Delete(void* obj) const=0;
			virtual void *Clone(const void* const obj) const=0;
			virtual bool  IsSameType(const std::type_info &) const=0;

			virtual const type_info Type() const = 0;
			
			virtual ~TypeInterface() { }
		};

		/// Type dependent implementation for TypeInterface.
		/// This class is created by Any to provide type related
		/// functions.
		template<class T_> class Type : public TypeInterface {
		public:
			virtual void* New() const {
				return new T_;
			}
			virtual void Delete(void *obj) const {
				delete static_cast<T_*>(obj);
			}
			virtual void *Clone(const void* const obj) const {
				T_ *n = new T_(obj);
				return n;
			}
			virtual bool IsSameType(const std::type_info &info) const {
				return info==typeid(T_);
			}
			virtual const type_info Type() const {
				return typeid(T_);
			}
		};

	public:
		/// Default constructor.
		/// Initializes and empty Any.
		Any() : content(nullptr),type(nullptr) { }

		/// Copy constructor.
		/// Requires type in the copied Any to be copy constructible.
		Any(const Any &any) {
			if(any.content) {
				type=(TypeInterface*)malloc(sizeof(TypeInterface));
				std::memcpy(type, any.type, sizeof(TypeInterface));
				content=type->Clone(any.content);
			} else {
				content=nullptr;
				type=nullptr;
			}
		}
		
		/// Move constructor.
		/// Requires type in the copied Any to be move constructible.
		Any(Any &&any) : content(nullptr), type(nullptr) {
			Swap(any);
		}

		/// Creates a new Any from the given data. This constructor
		/// duplicates the given data.
		/// Requires type in the copied Any to be copy constructible.
		template <class T_>
		explicit Any(const T_ &data) {
			type=new Type<T_>;
			content=type->Clone(&data);
		}

		/// Creates a new Any from the given data. This constructor
		/// moves the given data.
		/// Requires type in the moved Any to be move constructible.
		template <class T_>
		explicit Any(T_ &&data) {
			type=new Type<T_>;
			T_ *n=new T_(std::move(data));
			content=n;
		}

		/// Copies the information in the given Any. It requires
		/// type in the copied Any to be copy constructible.
		Any &operator =(const Any &any) {
			Clear();
			
			if(any.type) {
				type=(TypeInterface*)malloc(sizeof(TypeInterface));
				std::memcpy(type, any.type, sizeof(TypeInterface));
				content=type->Clone(any.content);
			}
			else {
				type=nullptr;
				content=nullptr;
			}

			return *this;
		}
		
		/// Moves the information in the given Any to this Any. This
		/// function directly moves the pointer therefore, no copying
		/// or moving performed on the original type.
		Any &operator =(Any &&any) {
			Clear();
			
			Swap(any);

			return *this;
		}

		/// Set the content of the Any to the given value. The value
		/// is duplicated. Requires T_ to be copy constructible.
		template<class T_>
		Any &operator =(const T_ &value) {
			Set(value);
		}

		/// Set the content of the Any to the given value. The value
		/// is moved. Requires T_ to be move constructible.
		template<class T_>
		Any &operator =(T_ &&value) {
			Set(std::move(value));
		}

		/// Swaps the contents of the current any with another. Does
		/// not perform copying or moving.
		void Swap(Any &other) {
			using std::swap;
			
			swap(type, other.type);
			swap(content, other.content);
		}

		/// Set the content of the Any to the given value. The value
		/// is duplicated. Requires T_ to be copy constructible.
		template <class T_>
		void Set(const T_ &data) {
			Clear();

			type=new Type<T_>;
			content=type->Clone(&data);
		}

		/// Set the content of the Any to the given value. The value
		/// is moved. Requires T_ to be move constructible.
		template <class T_>
		void Set(T_ &&data) {
			Clear();

			type=new Type<T_>;
			T_ *n=new T_(std::move(data));
			content=n;
		}

		/// 
		/// Clears the content of the any.
		void Clear() {
			if(content) {
				type->Delete(content);
				delete type;
				type=nullptr;
				content=nullptr;
			}
		}

		/// Returns the value contained with this any. If this Any is not
		/// const, this method could be used to move the object out of
		/// Any. It can also be used to modify the value contained within
		/// this Any.
		/// @throw std::runtime_error if Any is empty
		/// @throw std::bad_cast (debug only) if types do not match
		template <class T_>
		T_ &Get() {
#ifndef GORGON_FAST_ANY
			if(!type || !content) {
				throw std::runtime_error("Any storage is empty");
			}
#endif
#ifndef NDEBUG
			if(!type->IsSameType(typeid(T_))) {
				throw std::bad_cast("Cannot cast: not the same type");
			}
#endif

			return *static_cast<T_*>(content);
		}

		/// Returns the value contained with this any. If this Any is not
		/// const, this method could be used to move the object out of
		/// Any. It can also be used to modify the value contained within
		/// this Any.
		/// @throw std::runtime_error if Any is empty
		/// @throw std::bad_cast (debug only) if types do not match
		template <class T_>
		const T_ &Get() const {
#ifndef GORGON_FAST_ANY
			if(!type || !content) {
				throw std::runtime_error("Any storage is empty");
			}
#endif
#ifndef NDEBUG
			if(!type->IsSameType(typeid(T_))) {
				throw std::bad_cast("Cannot cast: not the same type");
			}
#endif

			return *static_cast<T_*>(content);
		}


		/// Unsafe version of Get. Should be used as last resort. Only
		/// const version is available.
		/// @warning Does not perform type check even in debug mode
		/// @throw std::runtime_error if Any is empty
		template <class T_>
		T_ UnsafeGet() const {
#ifndef GORGON_FAST_ANY
			if(!type || !content) {
				throw std::runtime_error("Any storage is empty");
			}
#endif

			return *static_cast<T_*>(content);
		}

		/// Checks whether the Any is the same type with the given type.
		template <class T_>
		bool TypeCheck() const {
			return type->IsSameType(typeid(T_));
		}

		/// Compares the contents of this Any to the given value. The value
		/// should be the same type as this Any. Even if it is possible, 
		/// no cross type comparison will be performed. You may use 
		/// `any.Get<originaltype>() == othertype_variable`
		/// @throw std::runtime_error if Any is empty
		/// @throw std::bad_cast (debug only) if types do not match
		template <class T_>
		bool operator ==(const T_ &content) const  {
#ifndef GORGON_FAST_ANY
			if(!type || !this->content) {
				throw std::runtime_error("Any storage is empty");
			}
#endif
#ifndef NDEBUG
			if(!type->IsSameType(typeid(T_))) {
				throw std::bad_cast("Cannot cast: not the same type");
			}
#endif

			return *static_cast<T_*>(this->content)==content;
		}

		/// Compares two Any variables. 
		/// @return true if both are empty or have their contents equal. 
		/// @throw std::bad_cast (debug only) if types do not match
		bool operator ==(const Any &content) const  {
			if(!content.content && !this->content) {
				return true;
			}
			else if(!content.content || !this->content) {
				return false;
			}
#ifndef NDEBUG
			if(type->Type() != content.type->Type()) {
				throw std::bad_cast("Cannot cast: not the same type");
			}
#endif

			return *static_cast<T_*>(this->content)==content;
		}

		/// Compares the contents of this Any to the given value. The value
		/// should be the same type as this Any. Even if it is possible, 
		/// no cross type comparison will be performed. You may use 
		/// `any.Get<originaltype>() == othertype_variable`
		/// @throw std::runtime_error if Any is empty
		/// @throw std::bad_cast (debug only) if types do not match
		template <class T_>
		bool operator !=(const T_ &content) const  {
			return !operator==(content);
		}

		/// Compares two Any variables. 
		/// @return true if both are empty or have their contents equal. 
		/// @throw std::bad_cast if types do not match
		bool operator !=(const Any &content) const  {
			return !operator==(content);
		}

		///
		/// Checks whether the Any is set 
		bool IsSet() const  {
			return content!=nullptr;
		}

		///
		/// Destructor
		~Any() {
			Clear();
		}

	protected:
		///
		/// Data stored in this any
		void *content;
		///
		/// Type of the data stored.
		TypeInterface *type;
	};
}
