#pragma once

#include "Base.h"
#include "Reader.h"
#include "../Geometry/Point.h"
#include "../Geometry/Size.h"

namespace Gorgon { namespace Resource {
	
	class File;
	
	class DataItem {
	public:
		virtual ~DataItem() { }
		
		/// Returns the Gorgon ID of this data object
		virtual GID::Type GetGID() const = 0;
		
		/// The name of the data item
		std::string Name;
		
		/// Saves the data item with header information to gorgon file
		virtual void Save(Writer &writer) {
			auto start=writer.WriteChunkStart(GetGID());
			writer.WriteStringWithSize(Name);
			SaveValue(writer);
			writer.WriteEnd(start);
		}
		
		/// Saves only the value of the data item to gorgon file. issizewritten controls whether the size of the object
		/// should be written for some data types (like blob and string)
		virtual void SaveValue(Writer &writer) = 0;
	};
	
	namespace internal {
		template<class T_>
		class DataImp {
		public:
			DataImp() = default; 
			
			DataImp(T_ v) : value(v) { }
			
			T_ Get() const { return value; }
			
			void Set(T_ val) { value = val; }
			
		protected:
			T_ value = T_();
		};
	}
	
	class IntegerData : public DataItem, private internal::DataImp<int> {
	public:
		IntegerData() {  }
		
		IntegerData(int v) : DataImp<int>(v) { }
		
		IntegerData(const std::string &name, int v) : DataImp<int>(v) {
			Name=name;
		}
		
		virtual GID::Type GetGID() const { return GID::Data_Int; }
		
		using internal::DataImp<int>::Get;
		using internal::DataImp<int>::Set;
		
		virtual void SaveValue(Writer &writer) override {
			writer.WriteInt32(value);
		}
	};
	
	class Data : public Base {
	public:
		
		/// Creates an empty Data
		Data() { }
		
		/// This constructor accepts a reflected struct and turns it into a resource data.
		/// The types are tried to be matched with the built-in data resources. Second parameter
		/// allows named reflection.
		template<class T_, class R_ = typename T_::ReflectionType>
		Data(const T_ &values, const R_ &reflectionobj = T_::Reflection()) {
			Append(values, reflectionobj);
		}
		
		/// This constructor accepts a reflected struct and turns it into a resource data.
		/// The types are tried to be matched with the built-in data resources. Multiple objects
		/// having same member names can be saved to a single data using second parameter, which
		/// appends a prefix to object members. Third parameter allows named reflection. 
		template<class T_, class R_ = typename T_::ReflectionType>
		Data(const T_ &values, const std::string &prefix, const R_ &reflectionobj = T_::Reflection()) {
			Append(values, prefix, reflectionobj);
		}
		
		virtual GID::Type GetGID() const override { 
			return GID::Data;
		}
		
		/// This function accepts a reflected struct and appends it to the resource data.
		/// The types are tried to be matched with the built-in data resources. Multiple objects
		/// having same member names can be saved to a single data using second parameter, which
		/// appends a prefix to object members. Third parameter allows named reflection. 
		template<class T_, class R_ = typename T_::ReflectionType>
		typename std::enable_if<R_::IsGorgonReflection, void>::type Append(const T_ &values, const R_ &reflectionobj = T_::Reflection()) {
			static_assert(R_::IsGorgonReflection, "The template argument R_ for this constructor should be reflection type of the struct.");
			
			Append(values, "", reflectionobj);
		}
		
		/// This function accepts a reflected struct and appends it to the resource data.
		/// The types are tried to be matched with the built-in data resources. Multiple objects
		/// having same member names can be saved to a single data using second parameter, which
		/// appends a prefix to object members. Third parameter allows named reflection. 
		template<class T_, class R_ = typename T_::ReflectionType>
		typename std::enable_if<R_::IsGorgonReflection, void>::type  Append(const T_ &values, const std::string &prefix, const R_ &reflectionobj = T_::Reflection()) {
			static_assert(R_::IsGorgonReflection, "The template argument R_ for this constructor should be reflection type of the struct.");
		}
		
		void Append(int value) {
			items.Add(new IntegerData(value));
		}
		
		void Append(const std::string &name, int value) {
			items.Add(new IntegerData(name, value));
		}
		
		void Append(unsigned long value);
		
		void Append(float value);
		
		void Append(double value);
		
		void Append(Geometry::Point p);
		
		void Append(Geometry::Size s);
		
		/// Transforms the members of this resource data to the given struct. Members are matched by name. 
		template<class T_, class R_ = typename T_::ReflectionType>
		T_ NamedTransform(const R_ &reflectionobj = T_::Reflection()) const;
		
		/// Transforms the members of this resource data to the given struct. Members are matched by name starting
		/// with prefix.
		template<class T_, class R_ = typename T_::ReflectionType>
		T_ NamedTransform(const std::string &prefix, const R_ &reflectionobj = T_::Reflection()) const;
		
		template<class T_>
		T_ Get(int index) const {
			static_assert(std::is_same<T_, int>::value, "Unknown data type.");
		}
		
		
		template<class T_>
		T_ Get(const std::string &name) const {
			static_assert(std::is_same<T_, int>::value, "Unknown data type.");
		}
		
		DataItem &GetItem(int index) const;
		
		DataItem &GetItem(const std::string &name) const;
		
		int FindIndex(const std::string &name) const;
		
		int GetCount() const;
		
		void Remove(int index);
		
		void Remove(const std::string &name);
		
		/// Ownership of the item is transferred to this Data
		void Add(DataItem &item);
		
		void Insert(DataItem &item, int before);
		
		
		static Data *LoadResource(std::weak_ptr<File> file, std::shared_ptr<Reader> data, unsigned long size);
		
	private:
		virtual void save(Writer &writer);
		
		Containers::Collection<DataItem> items;
	};
	
	template<>
	inline int Data::Get<int>(int index) const {
		auto item=dynamic_cast<IntegerData&>(items[index]);
		
		return item.Get();
	}
	
} }

extern std::vector< int > v;
