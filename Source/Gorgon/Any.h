/// @file Any.h contains class Any. Any is a container for any type and
///	supports boxing, unboxing and copying; uses RTTI. Best be used with
/// built in types or POD structures


#pragma once

#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <typeinfo>
#include <algorithm>
#include <typeinfo>
#include "Utils/Assert.h"
#include "TMP.h"



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
	/// Currently cannot work with object without copy constructor.
	class Any {
	public:
		/// Default constructor.
		/// Initializes and empty Any.
		Any() : content(nullptr),type(nullptr) { }
		
		/// Unsafe! Constructs any from give raw data. Both typeinterface and data are duplicated.
		/// @warning Using this constructor might be dangerous
		Any(const TMP::RTTS &typeinterface, void *data) {
			type=typeinterface.Duplicate();
			content=type->Clone(data);
		}
		
		/// Unsafe! Constructs any from give raw data. typeinterface is duplicated. Ownership of
		/// data is taken.
		/// @warning Using this constructor might be dangerous
		Any(void *data, const TMP::RTTS &typeinterface) {
			type=typeinterface.Duplicate();
			content=type->Clone(data);
		}
		
		/// Creates a new Any from the given data. This constructor
		/// duplicates the given data.
		/// Requires type in the copied Any to be copy constructible.
		template <class T_>
		Any(const T_ &data) {
			using Type = typename std::decay<T_>::type;
			static_assert(!std::is_same<typename std::decay<typename std::remove_reference<T_>::type>::type, Any>::value,
						  "Something is wrong in here");

			type=new TMP::RTT<Type>;
			content=type->Clone(&data);
		}

		/// Copy constructor.
		/// Requires type in the copied Any to be copy constructible.
		Any(const Any &any) {
			if(any.content) {
				type=any.type->Duplicate();
				content=type->Clone(any.content);
			}
			else {
				content=nullptr;
				type=nullptr;
			}
		}


		/// Move constructor.
		/// Requires type in the copied Any to be move constructible.
		Any(Any &&any) : content(nullptr), type(nullptr) {
			Swap(any);
		}
		
		/// Returns TypeInfo used by current data
		TMP::RTTI &TypeInfo() const {
			ASSERT(type, "Any is not set");
			
			return *type;
		}
		
		/// Returns TypeInterface used by this any.
		TMP::RTTS *TypeServices() const {
			return type;
		}

		
		/// Copies the information in the given Any. It requires
		/// type in the copied Any to be copy constructible.
		Any &operator =(const Any &any) {
			Clear();
			
			if(any.type) {
				type=any.type->Duplicate();
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
			static_assert(!std::is_same<typename std::decay<typename std::remove_reference<T_>::type>::type, Any>::value,
						  "Something is wrong in here");
			Set(value);
			
			return *this;
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

			using Type = typename std::decay<T_>::type;
			
			type=new TMP::RTT<Type>;
			content=type->Clone(&data);
		}

		/// Set the content of the Any to the given value. The value
		/// is moved. Requires T_ to be move constructible.
		template <class T_>
		void Set(T_ &&data) {
			Clear();


			using Type = typename std::decay<T_>::type;
			
			type=new TMP::RTT<Type>;
			Type *n=new Type(std::move(data));
			content=n;
		}

		
		/// Clears the content of the any.
		void Clear() {
			if(content) {
				type->Delete(content);
				delete type;
				type=nullptr;
				content=nullptr;
			}
		}
		
		/// Unsafe! This function returns raw data contained within any.
		/// @warning this function is unsafe
		void *GetRaw() const {
			return content;
		}
		
		/// Unsafe! This function sets the raw data contained within any, without modifying its
		/// type data. data is duplicated.
		///@warning this function is unsafe
		void SetRaw(void *data) {
			if(content) type->Delete(content);
			
			content=type->Clone(data);
		}
		
		/// Unsafe! This function sets the raw data contained within any, while modifying its
		/// type data. type and data are duplicated.
		///@warning this function is unsafe
		void SetRaw(TMP::RTTS *type, void *data) {
			Clear();
			
			this->type=type->Duplicate();
			content=type->Clone(data);
		}
		
		/// Unsafe! This function sets the data contained within any with regular means. However, the
		/// type info is user supplied. Its primary aim is to be able to set pointer types from
		/// from void pointers. This can also be used as type casting.
		///@warning this function is unsafe
		template<class T_>
		void Set(const T_ &data, TMP::RTTS *type) {
			Clear();
			
			this->type=type->Duplicate();
			content=type->Clone(&data);
		}
		
		/// Unsafe! This function sets the raw data contained within any, while modifying its
		/// type data. type is duplicated, whereas data ownership is assumed.
		///@warning this function is unsafe
		void AssumeRaw(TMP::RTTS &type, void *data) {
			Clear();
			
			this->type=type.Duplicate();
			content=data;
		}
		
		/// Unsafe! Disowns the data contained in this any. The data is not freed. This function
		/// returns the raw pointer that is disowned
		void *Disown() {
			auto ret=content;
			
			delete type;
			type=nullptr;
			content=nullptr;
			
			return ret;
		}
		
		/// Unsafe! This function modifies type information of the data content. type is duplicated
		///@warning this function is extremely unsafe, basically, it performs reinterpret_cast
		void SetType(const TMP::RTTS &type) {
			if(this->type) delete this->type;
			this->type=type.Duplicate();
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
			
			ASSERT(type->IsSameType(typeid(T_)),
				"Type mismatch, orginal: "+type->Name()+", target: "+Utils::GetTypeName<T_>()
			);
#endif

			return *static_cast<T_*>(content);
		}
		
		std::string GetTypeName() const {
			if(!type) return "";
			
			return Utils::demangle(type->TypeInfo().name());
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
			
			ASSERT(type->IsSameType(typeid(T_)),
				"Type mismatch, orginal: "+type->Name()+", target: "+Utils::GetTypeName<T_>()
			);
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
		
		/// Checks whether the Any is the same type with the given type.
		bool IsSameType(const Any &other) const {
			return type->IsSameType(other.type->TypeInfo());
		}
		
		/// Checks if any contains a pointer
		bool IsPointer() const {
			return type->IsPointer();
		}

		/// Returns the pointer without type information
		void *Pointer() const {
			return *reinterpret_cast<void**>(content);
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
			
			ASSERT(type->IsSameType(typeid(T_)),
				"Type mismatch, orginal: "+type->Name()+", target: "+Utils::GetTypeName<T_>()
			);
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
			
#ifndef GORGON_FAST_ANY
			ASSERT(type->TypeInfo() != content.type->TypeInfo(),
				"Type mismatch, left: "+type->Name()+", right: "+content.type->Name()
			);
#endif

			return memcmp(this->content, content.content, type->GetSize())==0;
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
		TMP::RTTS *type;
	};
	

	///@cond internal
	struct CopyFreeAny {
		virtual ~CopyFreeAny() {}

		template<class T_>
		T_ &GetData() const {
			return *reinterpret_cast<T_*>(data);
		}

		void *data;
	};

	template<class T_>
	struct CopyFreeAny_impl : public CopyFreeAny {
		CopyFreeAny_impl(T_ data) : mydata(std::move(data)) {
			this->data = &mydata;
		}

		CopyFreeAny_impl(const CopyFreeAny_impl &) = delete;

		CopyFreeAny_impl(CopyFreeAny_impl &&other) {
			mydata = std::move(other.data);
		}

		CopyFreeAny_impl &operator =(CopyFreeAny_impl &&other) {
			mydata = std::move(other.data);

			return *this;
		}

		CopyFreeAny_impl &operator =(const CopyFreeAny_impl &other) = delete;

		virtual ~CopyFreeAny_impl() {}

		T_ mydata;
	};

	/// This function returns a reference!
	template<class T_>
	CopyFreeAny &MakeCopyFreeAny(T_ data) {
		return *new CopyFreeAny_impl<T_>(std::move(data));
	}
	///@endcond


}
