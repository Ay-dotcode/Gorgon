#include "DataItems.h"
#include "Data.h"


#include "File.h"

namespace Gorgon { namespace Resource {

	void DataItem::InitializeLoaders() {
		DataLoaders[GID::Data_Int]   = &IntegerData::Load;
		DataLoaders[GID::Data_Float] = &FloatData::Load;
		DataLoaders[GID::Data_Text]  = &TextData::Load;
		DataLoaders[GID::Data_Point] = &PointData::Load;
	}

	DataItem* IntegerData::Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, long unsigned int totalsize) {
#ifndef NDEBUG
		auto target = reader->Target(totalsize);
#endif

		std::string name=reader->ReadString();
		int value = reader->ReadInt32();

		ASSERT((bool)target, "Integer data size mismatch.");

		return new IntegerData(name, value);
	}


	DataItem* FloatData::Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, long unsigned int totalsize) {
#ifndef NDEBUG
		auto target = reader->Target(totalsize);
#endif

		std::string name=reader->ReadString();
		float value = reader->ReadFloat();

		ASSERT((bool)target, "Float data size mismatch.");

		return new FloatData(name, value);
	}


	DataItem* TextData::Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, long unsigned int totalsize) {
#ifndef NDEBUG
		auto target = reader->Target(totalsize);
#endif

		std::string name=reader->ReadString();
		std::string value = reader->ReadString();

		ASSERT((bool)target, "Text data size mismatch.");

		return new TextData(name, value);
	}

	DataItem* PointData::Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, long unsigned int totalsize) {
#ifndef NDEBUG
		auto target = reader->Target(totalsize);
#endif

		std::string name=reader->ReadString();
		int x=reader->ReadInt32();
		int y=reader->ReadInt32();

		Geometry::Point value{x, y};

		ASSERT((bool)target, "Point data size mismatch: should be 4, reported as "+String::From(totalsize));

		return new PointData(name, value);
	}

	DataItem* PointfData::Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, long unsigned int totalsize) {
#ifndef NDEBUG
		auto target = reader->Target(totalsize);
#endif

		std::string name=reader->ReadString();
		float x=reader->ReadFloat();
		float y=reader->ReadFloat();

		Geometry::Pointf value{x, y};

		ASSERT((bool)target, "Pointf data size mismatch: should be 4, reported as "+String::From(totalsize));

		return new PointfData(name, value);
	}

	DataItem* SizeData::Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, long unsigned int totalsize) {
#ifndef NDEBUG
		auto target = reader->Target(totalsize);
#endif

		std::string name=reader->ReadString();
		int w=reader->ReadInt32();
		int h=reader->ReadInt32();

		Geometry::Size value{w, h};

		ASSERT((bool)target, "Size data size mismatch: should be 4, reported as "+String::From(totalsize));

		return new SizeData(name, value);
	}

	DataItem* RectangleData::Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, long unsigned int totalsize) {
#ifndef NDEBUG
		auto target = reader->Target(totalsize);
#endif

		std::string name=reader->ReadString();
		int x=reader->ReadInt32();
		int y=reader->ReadInt32();
		int w=reader->ReadInt32();
		int h=reader->ReadInt32();

		Geometry::Rectangle value{x,y, w,h};

		ASSERT((bool)target, "Rectangle data size mismatch: should be 4, reported as "+String::From(totalsize));

		return new RectangleData(name, value);
	}

	DataItem* BoundsData::Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, long unsigned int totalsize) {
#ifndef NDEBUG
		auto target = reader->Target(totalsize);
#endif

		std::string name=reader->ReadString();
		int l=reader->ReadInt32();
		int t=reader->ReadInt32();
		int r=reader->ReadInt32();
		int b=reader->ReadInt32();

		Geometry::Bounds value{l,t,r,b};

		ASSERT((bool)target, "Bounds data size mismatch: should be 4, reported as "+String::From(totalsize));

		return new BoundsData(name, value);
	}

	DataItem* MarginsData::Load(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, long unsigned int totalsize) {
#ifndef NDEBUG
		auto target = reader->Target(totalsize);
#endif

		std::string name=reader->ReadString();
		int l=reader->ReadInt32();
		int t=reader->ReadInt32();
		int r=reader->ReadInt32();
		int b=reader->ReadInt32();

		Geometry::Margins value{l,t,r,b};

		ASSERT((bool)target, "Margins data size mismatch: should be 4, reported as "+String::From(totalsize));

		return new MarginsData(name, value);
	}


	std::map<GID::Type, DataItem::LoaderFn> DataItem::DataLoaders;

} }