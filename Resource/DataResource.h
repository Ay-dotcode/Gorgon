#pragma once

#include "GRE.h"
#include "ResourceBase.h"
#include "../Utils/Margins.h"
#include "../Utils/Size2D.h"
#include "../Engine/Font.h"

#include <stdexcept>

namespace gge { namespace resource {
	class File;
	
	////This function loads a text resource from the given file
	ResourceBase *LoadDataResource(File* File, FILE* Data, int Size);

	////This is the data types that a data resource can contain
	enum DataTypes {
		////4 byte Integer data
		DT_Integer,
		////Floating point (4 byte)
		DT_Float,
		////String data
		DT_String,
		////4 byte integer point data
		DT_Point,
		////4 byte integer rectangle data
		DT_Rectangle,
		////Resource Link
		DT_Link,
	};

	////Data resource interface
	class IData : public ResourceBase {
	public:
		virtual bool Save(File *File, FILE *Data) { return false; }
		string name;

		virtual void Prepare(File *File) { }
	};

	////Integer data
	class IntegerData : public IData {
	public:
		////02030C02h (Basic, Data resource, Data types, integer)
		virtual int getGID() { return GID_DATAARRAY_INT; }
		int value;

		IntegerData(int value) { this->value=value; }
		operator int() { return value; }
	};

	////Float data
	class FloatData : public IData {
	public:
		////02030C03h (Basic, Data resource, Data types, float)
		virtual int getGID() { return GID_DATAARRAY_FLOAT; }
		float value;

		FloatData(float value) { this->value=value; }
		operator float() { return value; }
	};

	////String data
	class StringData : public IData {
	public:
		////02030C01h (Basic, Data resource, Data types, text)
		virtual int getGID() { return GID_DATAARRAY_TEXT; }
		string value;

		StringData(string value) { this->value=value; }
		operator string() { return value; }
		operator const char *() { return value.c_str(); }
	};

	////Point data
	class PointData : public IData {
	public:
		////02030C04h (Basic, Data resource, Data types, point)
		virtual int getGID() { return GID_DATAARRAY_POINT; }
		Point value;

		PointData(Point value) { this->value=value; }

		operator Point() { return value; }
		operator Size2D() { return value; }
	};

	////Rectangle data
	class RectangleData : public IData {
	public:
		////02030C05h (Basic, Data resource, Data types, rectangle)
		virtual int getGID() { return GID_DATAARRAY_RECT; }
		gge::Rectangle value;

		RectangleData(gge::Rectangle value)	{ this->value=value; }
		RectangleData(Bounds value)		{ this->value=value; }

		Bounds getBounds() { return (Bounds)value; }
		operator Bounds() { return (Bounds)value; }
		operator Rectangle() { return value; }
		operator Margins() { return Margins(value.Left,value.Top,value.Width,value.Height); }
	};

	////Link data
	class LinkData : public IData {
	public:
		////02030C02h (Basic, Data resource, Data types, integer)
		virtual int getGID() { return GID_DATAARRAY_LINK; }
		int getObjectGID() { if(value) return value->getGID(); return 0; }
		ResourceBase *value;
		utils::SGuid guid;

		LinkData(utils::SGuid guid) : value(NULL) { this->guid=guid; }
		operator ResourceBase *() { return value; }
		operator ResourceBase &() { if(!value) throw std::runtime_error("Target is not set"); return *value; }
		ResourceBase &Get() { if(!value) throw std::runtime_error("Target is not set"); return *value; }

		virtual void Prepare(File *File);
	};

	//Font data
	class FontData : public IData {
	public:
		////03300C01h (Gaming, FontTheme, Data types, font)
		virtual int getGID() { return 0x03300C01; }
		Font value;
		FontInitiator initiator;

		FontData(Font &font) { value=font; }
		FontData(FontInitiator &font) { initiator=font; }
		operator Font() { return value; }
		virtual void Prepare(File *File) {
			value=(Font)initiator;
		}
	};

	////This is data resource which holds an array of basic data types. These types are
	/// integer, float, string, point and rectangle.
	class DataResource : public ResourceBase {
		friend ResourceBase *LoadDataResource(File* File, FILE* Data, int Size);
	public:
		////Data collection
		utils::Collection<IData> Data;

		////Adds a new integer value to this resource
		IntegerData *Add(int value) { return dynamic_cast<IntegerData*>(Data[Data.Add(new IntegerData(value))]); }
		////Adds a new float value to this resource
		FloatData   *Add(float value) { return dynamic_cast<FloatData*>(Data[Data.Add(new FloatData(value))]); }
		////Adds a new string value to this resource
		StringData  *Add(string value) { return dynamic_cast<StringData*>(Data[Data.Add(new StringData(value))]); }
		////Adds a new point value to this resource
		PointData  *Add(Point value) { return dynamic_cast<PointData*>(Data[Data.Add(new PointData(value))]); }
		////Adds a new rectangle to this resource
		RectangleData  *Add(gge::Rectangle value) { return dynamic_cast<RectangleData*>(Data[Data.Add(new RectangleData(value))]); }
		////Adds a new rectangle to this resource
		RectangleData  *Add(Bounds value) { return dynamic_cast<RectangleData*>(Data[Data.Add(new RectangleData(value))]); }
		////Adds a new rectangle to this resource
		FontData	   *Add(Font value) { return dynamic_cast<FontData*>(Data[Data.Add(new FontData(value))]); }
		////Adds a new rectangle to this resource
		FontData	   *Add(FontInitiator value) { return dynamic_cast<FontData*>(Data[Data.Add(new FontData(value))]); }
		////Adds a new integer value to this resource
		LinkData *Add(utils::SGuid value) { return dynamic_cast<LinkData*>(Data[Data.Add(new LinkData(value))]); }

		////Returns item at index
		IData *operator [] (int Index) { return Data[Index]; }
		////Returns integer at index
		int getInt(int Index) { return dynamic_cast<IntegerData*>(Data[Index])->value; }
		////Returns float at index
		float getFloat(int Index) { return dynamic_cast<FloatData*>(Data[Index])->value; }
		////Returns string at index
		string getString(int Index) { return dynamic_cast<StringData*>(Data[Index])->value; }
		////Returns char array at index
		const char *getText(int Index) { return dynamic_cast<StringData*>(Data[Index])->value.data(); }
		////Returns point at index
		Point getPoint(int Index) { return dynamic_cast<PointData*>(Data[Index])->value; }
		////Returns size at index
		Size2D getSize2D(int Index) { return dynamic_cast<PointData*>(Data[Index])->value; }
		////Returns rectangle at index
		gge::Rectangle getRectangle(int Index) { return dynamic_cast<RectangleData*>(Data[Index])->value; }
		////Returns margins at index
		gge::Margins getMargins(int Index) { return *dynamic_cast<RectangleData*>(Data[Index]); }
		////Returns bounds at index
		Bounds getBounds(int Index) { return dynamic_cast<RectangleData*>(Data[Index])->getBounds(); }
		////Returns resource object from a link
		ResourceBase &getLink(int Index) { return dynamic_cast<LinkData*>(Data[Index])->Get(); }
		////Returns font object
		Font getFont(int Index) { return dynamic_cast<FontData*>(Data[Index])->value; }
		////Returns number of items in the array
		int getCount() { return Data.getCount(); }
		
		////02030000h (Basic, Data resource)
		virtual int getGID() { return GID_DATAARRAY; }
		////Currently does nothing
		virtual bool Save(File *File, FILE *Data) { return false; }

		virtual ~DataResource() { Data.Destroy(); ResourceBase::~ResourceBase(); }

		virtual void Prepare(GGEMain *main) { foreach(IData, data, Data) data->Prepare(file); }

	protected:
		File *file;
	};
} }
