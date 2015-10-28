#pragma once

#include "Base.h"
#include "Reader.h"
#include "DataItems.h"

#include <functional>

namespace Gorgon { namespace Resource {
	
	class File;
	
	class Data : public Base {
	public:
		
		using Iterator = Containers::Collection<DataItem>::Iterator;

		using ConstIterator = Containers::Collection<DataItem>::ConstIterator;

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
			Append("#"+String::From(items.GetCount()), value);
		}
		
		void Append(const std::string &name, int value) {
			items.Add(new IntegerData(name, value));
		}

		void Append(float value) {
			Append("#"+String::From(items.GetCount()), value);
		}

		void Append(const std::string &name, float value) {
			items.Add(new FloatData(name, value));
		}

		void Append(const std::string &value) {
			Append("#"+String::From(items.GetCount()), value);
		}

		void Append(const std::string &name, const std::string &value) {
			items.Add(new TextData(name, value));
		}

		void Append(const char *value) {
			Append("#"+String::From(items.GetCount()), value);
		}

		void Append(const std::string &name, const char *value) {
			items.Add(new TextData(name, value));
		}

		void Append(Geometry::Point value) {
			Append("#"+String::From(items.GetCount()), value);
		}

		void Append(const std::string &name, Geometry::Point value) {
			items.Add(new PointData(name, value));
		}

		void Append(Geometry::Pointf value) {
			Append("#"+String::From(items.GetCount()), value);
		}

		void Append(const std::string &name, Geometry::Pointf value) {
			items.Add(new PointfData(name, value));
		}

		void Append(Geometry::Size value) {
			Append("#"+String::From(items.GetCount()), value);
		}

		void Append(const std::string &name, Geometry::Size value) {
			items.Add(new SizeData(name, value));
		}

		void Append(Geometry::Rectangle value) {
			Append("#"+String::From(items.GetCount()), value);
		}

		void Append(const std::string &name, Geometry::Rectangle value) {
			items.Add(new RectangleData(name, value));
		}

		void Append(Geometry::Bounds value) {
			Append("#"+String::From(items.GetCount()), value);
		}

		void Append(const std::string &name, Geometry::Bounds value) {
			items.Add(new BoundsData(name, value));
		}

		void Append(Geometry::Margins value) {
			Append("#"+String::From(items.GetCount()), value);
		}

		void Append(const std::string &name, Geometry::Margins value) {
			items.Add(new MarginsData(name, value));
		}

		void Append(Base *value) {
			Append("#"+String::From(items.GetCount()), value);
		}

		void Append(const std::string &name, Base *value) {
			items.Add(new ObjectData(name, value));
		}

		void Append(Base &value) {
			Append(&value);
		}

		void Append(const std::string &name, Base &value) {
			Append(name, &value);
		}

		/// Transforms the members of this resource data to the given struct. Members are matched by name. 
		template<class T_, class R_ = typename T_::ReflectionType>
		T_ NamedTransform(const R_ &reflectionobj = T_::Reflection()) const;
		
		/// Transforms the members of this resource data to the given struct. Members are matched by name starting
		/// with prefix.
		template<class T_, class R_ = typename T_::ReflectionType>
		T_ NamedTransform(const std::string &prefix, const R_ &reflectionobj = T_::Reflection()) const;
		
		/// Returns the data with the given index; use GetObject in order to get resource objects.
		template<class T_>
		T_ Get(int index) const {
			static_assert(std::is_same<T_, int>::value, "Unknown data type.");
		}
		
		/// Returns the data with the given index; use GetObject in order to get resource objects.
		template<class T_>
		T_ Get(const std::string &name) const {
			static_assert(std::is_same<T_, int>::value, "Unknown data type.");
		}

		template<class T_>
		T_ &GetObject(int index) const {
			auto &item=dynamic_cast<ObjectData&>(items[index]);

			return item.Get<T_>();
		}

		template<class T_>
		T_ &GetObject(const std::string &name) const {
			Utils::NotImplemented();
		}

		ConstIterator begin() const {
			return items.begin();
		}

		ConstIterator end() const {
			return items.end();
		}

		Iterator begin() {
			return items.begin();
		}

		Iterator end() {
			return items.end();
		}

		DataItem &GetItem(int index) const {
			return items[index];
		}
		
		DataItem &GetItem(const std::string &name) const {
			for(auto &item : items) {
				if(item.Name==name) return item;
			}

			throw std::runtime_error("Cannot find the item requested: "+name);
		}
		
		int FindIndex(const std::string &name) const;
		
		int GetCount() const;
		
		void Remove(int index);
		
		void Remove(const std::string &name);
		
		/// Ownership of the item is transferred to this Data
		void Add(DataItem &item);
		
		void Insert(DataItem &item, int before);
		
		
		static Data *LoadResource(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long totalsize);
		
	private:
		virtual void save(Writer &writer);
		
		Containers::Collection<DataItem> items;
		
		
	};

	template<>
	inline int Data::Get<int>(int index) const {
		auto &item=dynamic_cast<IntegerData&>(items[index]);

		return item.Get();
	}

	template<>
	inline float Data::Get<float>(int index) const {
		auto &item=dynamic_cast<FloatData&>(items[index]);

		return item.Get();
	}

	template<>
	inline std::string Data::Get<std::string>(int index) const {
		auto &item=dynamic_cast<TextData&>(items[index]);

		return item.Get();
	}

	template<>
	inline Geometry::Point Data::Get<Geometry::Point>(int index) const {
		auto &item=dynamic_cast<PointData&>(items[index]);

		return item.Get();
	}

	template<>
	inline Geometry::Pointf Data::Get<Geometry::Pointf>(int index) const {
		auto &item=dynamic_cast<PointfData&>(items[index]);

		return item.Get();
	}

	template<>
	inline Geometry::Size Data::Get<Geometry::Size>(int index) const {
		auto &item=dynamic_cast<SizeData&>(items[index]);

		return item.Get();
	}

	template<>
	inline Geometry::Rectangle Data::Get<Geometry::Rectangle>(int index) const {
		auto &item=dynamic_cast<RectangleData&>(items[index]);

		return item.Get();
	}

	template<>
	inline Geometry::Bounds Data::Get<Geometry::Bounds>(int index) const {
		auto &item=dynamic_cast<BoundsData&>(items[index]);

		return item.Get();
	}

	template<>
	inline Geometry::Margins Data::Get<Geometry::Margins>(int index) const {
		auto &item=dynamic_cast<MarginsData&>(items[index]);

		return item.Get();
	}

} }

extern std::vector< int > v;
