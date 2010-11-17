#pragma once

#include "GRE.h"
#include "ResourceBase.h"

namespace gre {
	class ResourceFile;
	
	////This function loads a text resource from the given file
	ResourceBase *LoadDataResource(ResourceFile* File, FILE* Data, int Size);

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
		DT_Rectangle
	};

	////Data resource interface
	class IData : public ResourceBase {
	public:
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }
		string name;
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
	};

	////This is data resource which holds an array of basic data types. These types are
	/// integer, float, string, point and rectangle.
	class DataResource : public ResourceBase {
		friend ResourceBase *LoadDataResource(ResourceFile* File, FILE* Data, int Size);
	public:
		////Data collection
		Collection<IData> Data;

		////Adds a new integer value to this resource
		IntegerData *Add(int value) { return (IntegerData*)Data[Data.Add(new IntegerData(value))]; }
		////Adds a new float value to this resource
		FloatData   *Add(float value) { return (FloatData*)Data[Data.Add(new FloatData(value))]; }
		////Adds a new string value to this resource
		StringData  *Add(string value) { return (StringData*)Data[Data.Add(new StringData(value))]; }
		////Adds a new point value to this resource
		PointData  *Add(Point value) { return (PointData*)Data[Data.Add(new PointData(value))]; }
		////Adds a new rectangle to this resource
		RectangleData  *Add(gge::Rectangle value) { return (RectangleData*)Data[Data.Add(new RectangleData(value))]; }
		////Adds a new rectangle to this resource
		RectangleData  *Add(Bounds value) { return (RectangleData*)Data[Data.Add(new RectangleData(value))]; }

		////Returns item at index
		IData *operator [] (int Index) { return Data[Index]; }
		////Returns integer at index
		int getInt(int Index) { return ((IntegerData*)Data[Index])->value; }
		////Returns float at index
		float getFloat(int Index) { return ((FloatData*)Data[Index])->value; }
		////Returns string at index
		string getString(int Index) { return ((StringData*)Data[Index])->value; }
		////Returns char array at index
		const char *getText(int Index) { return ((StringData*)Data[Index])->value.data(); }
		////Returns point at index
		Point getPoint(int Index) { return ((PointData*)Data[Index])->value; }
		////Returns rectangle at index
		gge::Rectangle getRectangle(int Index) { return ((RectangleData*)Data[Index])->value; }
		////Returns bounds at index
		Bounds getBounds(int Index) { return ((RectangleData*)Data[Index])->getBounds(); }
		
		////02030000h (Basic, Data resource)
		virtual int getGID() { return GID_DATAARRAY; }
		////Currently does nothing
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }

		virtual ~DataResource() { Data.Destroy(); ResourceBase::~ResourceBase(); }
	};
}