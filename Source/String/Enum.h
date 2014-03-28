/// @file Enum.h contains Enum class that allows C++ 
/// enums to have string capabilities

#pragma once
#include <string>

namespace Gorgon { namespace String {

	
	/// Allows C++ enum to have string capabilities. These are,
	/// conversion to string, conversion from string and add all members.
	/// Additionally, this class provides interfaces for Listbox and the
	/// like to convert enumeration to string without any need of extra
	/// coding. Additional work has to be done to use enumerations with
	/// this method. First of all, enum should be surrounded by a class.
	/// This class should be supplied as template parameter. The enum
	/// should be named as `Type` or typedef'd as `Type`. Additionally
	/// This wrapper class should have a `static const char *` or `static
	/// std::string` typed `Names` array, that contains string names
	/// for enumeration items. Also the wrapper class should contain an
	/// item named `end` which should be last enumeration item +1
	/// This value can also be a part of the enum but not necessarily.
	///
	/// **Example 1**
	/// @code
	/// //MyEnum.h
	/// class MyEnumWrapper {
	/// public:
	///     enum Type { Zero = 0, One, Two };
	///     static const int   end;
	///     static const char *Names[3];
	/// };
	/// typedef Gorgon::String::Enum<MyEnumWrapper> MyEnum;
	///
	/// //MyEnum.cpp
	/// int MyEnumWrapper::end=MyEnumWrapper::two + 1;
	/// const char *MyEnumWrapper::Names = {"Zero", "One", "Two" };
	///
	/// //Somewhere else
	/// MyEnum::Type value=MyEnum::One;
	/// std::cout<<MyEnum::ToString(value)<<std::endl;
	///
	/// @endcode
	///
	/// **Example 2**
	/// @code
	/// enum MyEnum { Zero = 0, One, Two };
	/// class MyEnumWrapper : public MyEnum {
	/// public:
	///     typedef MyEnum     Type;
	///     static const int   end;
	///     static const char *Names[3];
	/// };
	/// typedef Gorgon::String::Enum<MyEnumWrapper> MyCapableEnum;
	///
	/// //MyEnum.cpp
	/// int MyEnumWrapper::end=MyEnumWrapper::two + 1;
	/// const char *MyEnumWrapper::Names = {"Zero", "One", "Two" };
	///
	/// //Somewhere else
	/// MyEnum value=One;
	/// std::cout<<MyCapableEnum::ToString(value)<<std::endl;
	/// @endcode
	template<class T_>
	class Enum : public T_ {
	public:

		/// Converts given type to string, if the given value is out of
		/// range, empty string is returned.
		static std::string ToString(const typename T_::Type &e) {
			if(e<T_::end && e>=0)
				return T_::Names[e];
			else
				return "";
		}

		/// This method can be supplied to Listbox and the like to allow the use
		/// of this enum without any extra coding
		static void ConvertToString(const typename T_::Type &e, std::string &s) {
			s=ToString(e);
		}

		/// Parses the given string to determine the enumeration value. Returns end
		/// if not the same. Comparison is case sensitive
		static typename T_::Type Parse(const std::string &s) {
			for(typename T_::Type e=(typename T_::Type)0;e<T_::end;e++)
				if(s==T_::Names[e])
					return e;
			
			return T_::end;
		}

		/// This method can be supplied to Combobox and the like to allow the use
		/// of this enum without any extra coding
		static void ParseTo(typename T_::Type &e, const std::string &s) {
			//TODO
			for(e=(typename T_::Type)0;e<T_::end;e++)
				if(s==T_::Names[e])
					return;
				
			e=T_::end;
		}

		/// Adds all elements of this Enum to a given collection.
		//TODO modify
		template<class C_>
		static void AddAll(C_ &collection) {
			for(int i=0;i<T_::end;i++)
				collection.Add((typename T_::Type)i);
		}
		
		/// Helper function to work with Gorgon::String::From
		friend std::string From<T_>(const T_::Type &e) {
			return Enum<T_>ToString(e);
		}
		
	};

}}
