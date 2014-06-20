/// @file Enum.h contains Enum class that allows C++ enums to have string capabilities. 

#pragma once
#include <string>
#include <iostream>

	class gorgon__no_enum_trait {
	public:
		static const bool isupgradedenum=false;
	};

	
	template<class T_>
	gorgon__no_enum_trait gorgon__enum_trait_locator(T_);

namespace Gorgon {

	/// @cond INTERNAL

	/// Expands user declared enumtraits for additional capabilities
	template<class T_>
	class expandedenumtraits {
	public:
		static_assert(decltype(gorgon__enum_trait_locator(T_()))::isupgradedenum, "Should be an enum");
		
		expandedenumtraits() {
			T_ prev=traits.mapping.begin()->first;
			listing.push_back(prev);
			for(auto &p : traits.mapping) {
				reversemapping.insert(std::make_pair(p.second, p.first));
				if(p.first!=prev) {
					listing.push_back(p.first);
					prev=p.first;
				}
			}
		}
		
		bool lookupstring(T_ e, std::string &s) const {
			auto item=traits.mapping.find(e);
			if(item==traits.mapping.end()) return false;
			s=item->second;
			return true;
		}
		
		bool lookupvalue(const std::string &s, T_ &e) const {
			auto item=reversemapping.find(s);
			if(item==reversemapping.end()) return false;
			e=item->second;
			return true;
		}
		
		typename std::vector<T_>::const_iterator begin() const {
			return listing.begin();
		}
		
		typename std::vector<T_>::const_iterator end() const {
			return listing.end();
		}
		
	private:
		const decltype(gorgon__enum_trait_locator(T_())) traits;
		std::map<std::string, T_> reversemapping;
		std::vector<T_> listing;
	};

	/// This class performs instanced member to static conversion
	template<class T_>
	class staticenumtraits {
	public:
		static_assert(decltype(gorgon__enum_trait_locator(T_()))::isupgradedenum, "Should be an enum");
		
		static bool lookupstring(T_ e, std::string &s) {
			return traits.lookupstring(e, s);
		}
		
		static bool lookupvalue(const std::string &s, T_ &e) {
			return traits.lookupvalue(s, e);
		}
		
		static typename std::vector<T_>::const_iterator begin() {
			return traits.begin();
		}
		
		static typename std::vector<T_>::const_iterator end() {
			return traits.end();
		}
		
		static const expandedenumtraits<T_> traits;
	};

	template<class T_> 
	const expandedenumtraits<T_> staticenumtraits<T_>::traits;
	
	template<class T_>
	class enum_type_id {};
	
	/// @endcond

	/// This macro converts a regular C++ enumeration in to an enumarable, streamable, and parsable
	/// enumeration by assigning one or more strings to enum values. This version should be invoked
	/// in the same namespace as the original enumeration. If this is not possible, you may use
	/// DefineEnumStringsNS macro. You may use DefineEnumStringsCN to modify readable typename
	/// @see Enumerate 
	/// @see Parse
	#define DefineEnumStrings(type, ...) \
	class gorgon_enumtraits_##type {\
	public:\
		static const bool isupgradedenum=true;\
		\
		static const char *name() { return #type; }\
		\
		const std::multimap<type, std::string> mapping={__VA_ARGS__};\
	};\
	gorgon_enumtraits_##type gorgon__enum_trait_locator(type);

	/// This macro converts a regular C++ enumeration in to an enumarable, streamable, and parsable
	/// enumeration by assigning one or more strings to enum values. This version should be invoked
	/// in the same namespace as the original enumeration. If this is not possible, you may use
	/// DefineEnumStringsNSCN macro. This variant allows modification of readable typename
	/// @see Enumerate 
	/// @see Parse
	#define DefineEnumStringsCN(type, name, ...) \
	class gorgon_enumtraits_##type {\
	public:\
		static const bool isupgradedenum=true;\
		\
		static const char *name() { return #name; }\
		\
		const std::multimap<type, std::string> mapping={__VA_ARGS__};\
	};\
	gorgon_enumtraits_##type gorgon__enum_trait_locator(type);

	/// This macro converts a regular C++ enumeration in to an enumarable, streamable, and parsable
	/// enumeration by assigning one or more strings to enum values. This variant allows namespace.
	/// Use DefineEnumStringsNSCN to set custom typename
	/// @see Enumerate 
	/// @see Parse
	#define DefineEnumStringsNS(ns, type, ...) \
	class gorgon_enumtraits_##type {\
	public:\
		static const bool isupgradedenum=true;\
		\
		static const char *name() { return #type; }\
		\
		const std::multimap<ns::type, std::string> mapping={__VA_ARGS__};\
	};\
	gorgon_enumtraits_##type gorgon__enum_trait_locator(ns::type);

	/// This macro converts a regular C++ enumeration in to an enumarable, streamable, and parsable
	/// enumeration by assigning one or more strings to enum values. This variant allows namespace
	/// and custom readable typename.
	/// @see Enumerate 
	/// @see Parse
	#define DefineEnumStringsNSCN(ns, type, name, ...) \
	class gorgon_enumtraits_##type {\
	public:\
		static const bool isupgradedenum=true;\
		\
		static const char *name() { return name; }\
		\
		const std::multimap<ns::type, std::string> mapping={__VA_ARGS__};\
	};\
	gorgon_enumtraits_##type gorgon__enum_trait_locator(ns::type);
	
	/// Allows enumeration of upgraded enums using range based for.
	/// @code
	/// for(auto e : Enumerate<MyEnum>()) {
	///     std::cout<<e<<std::endl;
	/// }
	/// @endcode
	template<class T_>
	typename std::enable_if<decltype(gorgon__enum_trait_locator(T_()))::isupgradedenum, enum_type_id<T_>>::type Enumerate() {
		return enum_type_id<T_>();
	}

	template <class T_>
	typename std::vector<T_>::const_iterator begin(enum_type_id<T_> &) {
		return staticenumtraits<T_>::begin();
	}

	template <class T_>
	typename std::vector<T_>::const_iterator end(enum_type_id<T_> &) {
		return staticenumtraits<T_>::end();
	}

	namespace String {
		template<class T_>
		typename std::enable_if<decltype(gorgon__enum_trait_locator(T_()))::isupgradedenum, T_>::type To(const std::string &text) {
			T_ e;
			if(!staticenumtraits<T_>::lookupvalue(text, e)) return T_();
			return e;
		}
		
		template<class T_>
		typename std::enable_if<decltype(gorgon__enum_trait_locator(T_()))::isupgradedenum, std::string>::type From(const T_ &e) {
			std::string s;
			if(!staticenumtraits<T_>::lookupstring(e, s)) return "";
			return s;
		}
		
		template<class T_>
		typename std::enable_if<decltype(gorgon__enum_trait_locator(T_()))::isupgradedenum, T_>::type Parse(const std::string &text) {
			T_ e;
			if(!staticenumtraits<T_>::lookupvalue(text, e)) {
				throw ParseError(20001, std::string("Given string is not a valid ")+decltype(gorgon__enum_trait_locator(T_()))::name());
			}
			return e;
		}
	}
}

/// Stream writer for upgraded enums
template<class T_>
typename std::enable_if<decltype(gorgon__enum_trait_locator(T_()))::isupgradedenum, std::ostream&>::type operator <<(std::ostream &out, const T_ &e) {
	std::string s;
	if(!Gorgon::staticenumtraits<T_>::lookupstring(e, s)) return out;
	out<<s;
	
	return out;
}

/// Stream reader for upgraded enums
template<class T_>
typename std::enable_if<decltype(gorgon__enum_trait_locator(T_()))::isupgradedenum, std::istream&>::type operator >>(std::istream &in, const T_ &e) {
	std::string s;
	e=T_();
	in>>s;
	if(!Gorgon::staticenumtraits<T_>::lookupvalue(s, e)) in.setstate(in.badbit);
	
	return in;
}
