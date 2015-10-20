#pragma once

#include "Base.h"
#include "Reader.h"
#include "../Geometry/Point.h"
#include "../Geometry/Size.h"
#include "../Geometry/Rectangle.h"
#include "../Geometry/Bounds.h"
#include "../Geometry/Margins.h"

#include <functional>

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

		/// Returns the contents of this data item to the requested type
		template <class T_>
		T_ Get() const {
			static_assert(std::is_same<T_, int>::value, "Unknown data type.");
		}

		/// Converts the contents of this data to string
		virtual std::string ToString() const = 0;
		
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
		IntegerData() {}

		IntegerData(int v) : DataImp<int>(v) {}

		IntegerData(const std::string &name, int v) : DataImp<int>(v) {
			Name=name;
		}

		virtual GID::Type GetGID() const { return GID::Data_Int; }

		using internal::DataImp<int>::Get;
		using internal::DataImp<int>::Set;

		virtual void SaveValue(Writer &writer) override {
			writer.WriteInt32(value);
		}
		
		static DataItem *Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long totalsize);

		virtual std::string ToString() const override {
			return String::From(value);
		}
	};

	class FloatData : public DataItem, private internal::DataImp<float> {
	public:
		FloatData() {}

		FloatData(float v) : DataImp<float>(v) {}

		FloatData(const std::string &name, float v) : DataImp<float>(v) {
			Name=name;
		}

		virtual GID::Type GetGID() const { return GID::Data_Float; }

		using internal::DataImp<float>::Get;
		using internal::DataImp<float>::Set;

		virtual void SaveValue(Writer &writer) override {
			writer.WriteFloat(value);
		}

		static DataItem *Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long totalsize);

		virtual std::string ToString() const override {
			return String::From(value);
		}
	};

	class TextData : public DataItem, private internal::DataImp<std::string> {
	public:
		TextData() {}

		TextData(std::string v) : DataImp<std::string>(v) {}

		TextData(const std::string &name, const std::string &v) : DataImp<std::string>(v) {
			Name=name;
		}

		virtual GID::Type GetGID() const { return GID::Data_Text; }

		using internal::DataImp<std::string>::Get;
		using internal::DataImp<std::string>::Set;

		virtual void SaveValue(Writer &writer) override {
			writer.WriteStringWithSize(value);
		}

		static DataItem *Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long totalsize);

		virtual std::string ToString() const override {
			return value;
		}
	};

	class PointData : public DataItem, private internal::DataImp<Geometry::Point> {
	public:
		PointData() {}

		PointData(Geometry::Point v) : DataImp<Geometry::Point>(v) {}

		PointData(const std::string &name, Geometry::Point v) : DataImp<Geometry::Point>(v) {
			Name=name;
		}

		virtual GID::Type GetGID() const { return GID::Data_Point; }

		using internal::DataImp<Geometry::Point>::Get;
		using internal::DataImp<Geometry::Point>::Set;

		virtual void SaveValue(Writer &writer) override {
			writer.WriteInt32(value.X);
			writer.WriteInt32(value.Y);
		}

		static DataItem *Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long totalsize);

		virtual std::string ToString() const override {
			return String::From(value);
		}
	};

	class PointfData : public DataItem, private internal::DataImp<Geometry::Pointf> {
	public:
		PointfData() {}

		PointfData(Geometry::Pointf v) : DataImp<Geometry::Pointf>(v) {}

		PointfData(const std::string &name, Geometry::Pointf v) : DataImp<Geometry::Pointf>(v) {
			Name=name;
		}

		virtual GID::Type GetGID() const { return GID::Data_Pointf; }

		using internal::DataImp<Geometry::Pointf>::Get;
		using internal::DataImp<Geometry::Pointf>::Set;

		virtual void SaveValue(Writer &writer) override {
			writer.WriteFloat(value.X);
			writer.WriteFloat(value.Y);
		}

		static DataItem *Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long totalsize);

		virtual std::string ToString() const override {
			return String::From(value);
		}
	};

	class SizeData : public DataItem, private internal::DataImp<Geometry::Size> {
	public:
		SizeData() {}

		SizeData(Geometry::Size v) : DataImp<Geometry::Size>(v) {}

		SizeData(const std::string &name, Geometry::Size v) : DataImp<Geometry::Size>(v) {
			Name=name;
		}

		virtual GID::Type GetGID() const { return GID::Data_Size; }

		using internal::DataImp<Geometry::Size>::Get;
		using internal::DataImp<Geometry::Size>::Set;

		virtual void SaveValue(Writer &writer) override {
			writer.WriteInt32(value.Width);
			writer.WriteInt32(value.Height);
		}

		static DataItem *Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long totalsize);

		virtual std::string ToString() const override {
			return String::From(value);
		}
	};

	class RectangleData : public DataItem, private internal::DataImp<Geometry::Rectangle> {
	public:
		RectangleData() {}

		RectangleData(Geometry::Rectangle v) : DataImp<Geometry::Rectangle>(v) {}

		RectangleData(const std::string &name, Geometry::Rectangle v) : DataImp<Geometry::Rectangle>(v) {
			Name=name;
		}

		virtual GID::Type GetGID() const { return GID::Data_Rectangle; }

		using internal::DataImp<Geometry::Rectangle>::Get;
		using internal::DataImp<Geometry::Rectangle>::Set;

		virtual void SaveValue(Writer &writer) override {
			writer.WriteInt32(value.X);
			writer.WriteInt32(value.Y);
			writer.WriteInt32(value.Width);
			writer.WriteInt32(value.Height);
		}

		static DataItem *Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long totalsize);

		virtual std::string ToString() const override {
			return String::From(value);
		}
	};

	class BoundsData : public DataItem, private internal::DataImp<Geometry::Bounds> {
	public:
		BoundsData() {}

		BoundsData(Geometry::Bounds v) : DataImp<Geometry::Bounds>(v) {}

		BoundsData(const std::string &name, Geometry::Bounds v) : DataImp<Geometry::Bounds>(v) {
			Name=name;
		}

		virtual GID::Type GetGID() const { return GID::Data_Bounds; }

		using internal::DataImp<Geometry::Bounds>::Get;
		using internal::DataImp<Geometry::Bounds>::Set;

		virtual void SaveValue(Writer &writer) override {
			writer.WriteInt32(value.Left);
			writer.WriteInt32(value.Top);
			writer.WriteInt32(value.Right);
			writer.WriteInt32(value.Bottom);
		}

		static DataItem *Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long totalsize);

		virtual std::string ToString() const override {
			return String::From(value);
		}
	};

	class MarginsData : public DataItem, private internal::DataImp<Geometry::Margins> {
	public:
		MarginsData() {}

		MarginsData(Geometry::Margins v) : DataImp<Geometry::Margins>(v) {}

		MarginsData(const std::string &name, Geometry::Margins v) : DataImp<Geometry::Margins>(v) {
			Name=name;
		}

		virtual GID::Type GetGID() const { return GID::Data_Margins; }

		using internal::DataImp<Geometry::Margins>::Get;
		using internal::DataImp<Geometry::Margins>::Set;

		virtual void SaveValue(Writer &writer) override {
			writer.WriteInt32(value.Left);
			writer.WriteInt32(value.Top);
			writer.WriteInt32(value.Right);
			writer.WriteInt32(value.Bottom);
		}

		static DataItem *Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long totalsize);

		virtual std::string ToString() const override {
			return String::From(value);
		}
	};

	inline std::ostream &operator <<(std::ostream &out, const DataItem &item) {
		out<<item.ToString();

		return out;
	}

	class Data : public Base {
	public:

		using LoaderFn=std::function<DataItem *(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long totalsize)>;
		
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

		DataItem &GetItem(int index) const;
		
		DataItem &GetItem(const std::string &name) const;
		
		int FindIndex(const std::string &name) const;
		
		int GetCount() const;
		
		void Remove(int index);
		
		void Remove(const std::string &name);
		
		/// Ownership of the item is transferred to this Data
		void Add(DataItem &item);
		
		void Insert(DataItem &item, int before);
		
		
		static Data *LoadResource(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long totalsize);
		
		static void InitializeLoaders();
		
		static std::map<GID::Type, LoaderFn> DataLoaders;
		
	private:
		virtual void save(Writer &writer);
		
		Containers::Collection<DataItem> items;
		
		
	};

	template<>
	inline int Data::Get<int>(int index) const {
		auto item=dynamic_cast<IntegerData&>(items[index]);

		return item.Get();
	}

	template<>
	inline float Data::Get<float>(int index) const {
		auto item=dynamic_cast<FloatData&>(items[index]);

		return item.Get();
	}

	template<>
	inline Geometry::Point Data::Get<Geometry::Point>(int index) const {
		auto item=dynamic_cast<PointData&>(items[index]);

		return item.Get();
	}

	template<>
	inline std::string Data::Get<std::string>(int index) const {
		auto item=dynamic_cast<TextData&>(items[index]);

		return item.Get();
	}


	template<>
	inline int DataItem::Get<int>() const {
		auto item=dynamic_cast<const IntegerData&>(*this);

		return item.Get();
	}

	template<>
	inline float DataItem::Get<float>() const {
		auto item=dynamic_cast<const FloatData&>(*this);

		return item.Get();
	}

	template<>
	inline Geometry::Point DataItem::Get<Geometry::Point>() const {
		auto item=dynamic_cast<const PointData&>(*this);

		return item.Get();
	}

	template<>
	inline std::string DataItem::Get<std::string>() const {
		auto item=dynamic_cast<const TextData&>(*this);

		return item.Get();
	}

} }

extern std::vector< int > v;
