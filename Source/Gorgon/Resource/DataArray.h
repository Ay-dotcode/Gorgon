#pragma once

#include <stdexcept>

#include "../Types.h"
#include "Base.h"
#include "../Geometry/Margins.h"
#include "../Geometry/Size.h"
#include "../Engine/Font.h"

#include "../Utils/OrderedCollection.h"

namespace gge { namespace resource {
	class File;

	class DataArray;
	
	////This function loads a text resource from the given file
	DataArray *LoadDataResource(File &File, std::istream &Data, int Size);

	////This is the data types that a data resource can contain
	enum DataTypes {
		////4 byte Integer data
		DT_Integer,
		////Floating utils::Point (4 byte)
		DT_Float,
		////String data
		DT_String,
		////4 byte integer utils::Point data
		DT_Point,
		////4 byte integer utils::Point data
		DT_Point2D,
		////4 byte integer rectangle data
		DT_Rectangle,
		////Resource Link
		DT_Link,
	};

	////Data resource interface
	class IData : public Base {
	public:
		std::string name;

		virtual void Prepare(File &File) { }
	};

	////Integer data
	class IntegerData : public IData {
	public:
		////02030C02h (Basic, Data resource, Data types, integer)
		virtual int GetGID() const { return GID::Data_Int; }
		int value;

		IntegerData(int value) { this->value=value; }
		operator int() { return value; }
	};

	class ColorData : public IntegerData {
	public:
		////02030D02h (Basic, Data resource, Data types, color)
		virtual int GetGID() const { return GID::Data_Color; }

		ColorData(graphics::RGBint value) : IntegerData(value) {  }
		operator graphics::RGBint() { return value; }
		ColorData &operator =(graphics::RGBint value) {
			IntegerData::value=value;

			return *this;
		}
	};

	////Float data
	class FloatData : public IData {
	public:
		////02030C03h (Basic, Data resource, Data types, float)
		virtual int GetGID() const { return GID::Data_Float; }
		float value;

		FloatData(float value) { this->value=value; }
		operator float() { return value; }
	};

	////String data
	class StringData : public IData {
	public:
		////02030C01h (Basic, Data resource, Data types, text)
		virtual int GetGID() const { return GID::Data_Text; }
		std::string value;

		StringData(std::string value) { this->value=value; }
		operator std::string() { return value; }
		operator const char *() { return value.c_str(); }
	};

	///utils::Point2D data
	class Point2DData: public IData {
	public:
		virtual int GetGID() const { return GID::Data_Point2D; }
		utils::Point2D value;

		Point2DData(utils::Point2D value) { this->value=value; }

		operator utils::Point2D() { return value; }
		operator utils::Size2D() { return value; }
	};

	////utils::Point data
	class PointData : public IData {
	public:
		////02030C04h (Basic, Data resource, Data types, utils::Point)
		virtual int GetGID() const { return GID::Data_Point; }
		utils::Point value;

		PointData(utils::Point value) { this->value=value; }

		operator utils::Point() { return value; }
		operator utils::Size() { return value; }
	};

	////utils::Size data
	class SizeData : public IData {
	public:
		////02030D03h (Basic, Data resource, Data types, utils::Point)
		virtual int GetGID() const { return GID::Data_Size; }
		utils::Size value;

		SizeData(utils::Size value) { this->value=value; }

		operator utils::Size() { return value; }
	};

	////Rectangle data
	class RectangleData : public IData {
	public:
		////02030C05h (Basic, Data resource, Data types, rectangle)
		virtual int GetGID() const { return GID::Data_Rect; }
		utils::Rectangle value;

		RectangleData(utils::Rectangle value)	{ this->value=value; }

		operator utils::Rectangle() { return value; }
		operator utils::Margins() { return utils::Margins(value.Left,value.Top,value.Width,value.Height); }
	};

	////Bounds data
	class BoundsData : public IData {
	public:
		////02030D04h (Basic, Data resource, Data types, rectangle)
		virtual int GetGID() const { return GID::Data_Bounds; }
		utils::Bounds value;

		BoundsData(utils::Bounds value)	{ this->value=value; }

		operator utils::Bounds() { return value; }
	};

	////Rectangle data
	class MarginsData : public IData {
	public:
		////02030C05h (Basic, Data resource, Data types, rectangle)
		virtual int GetGID() const { return GID::Data_Margins; }
		utils::Margins value;

		MarginsData(utils::Margins value)	{ this->value=value; }

		operator utils::Margins() { return value; }
	};

	////Link data
	class LinkData : public IData {
	public:
		////02030C02h (Basic, Data resource, Data types, integer)
		virtual int GetGID() const { return GID::Data_Link; }
		int getObjectGID() { if(value) return value->GetGID(); return 0; }
		Base *value;
		utils::SGuid guid;

		LinkData(utils::SGuid guid) : value(NULL) { this->guid=guid; }
		operator Base *() { return value; }
		operator Base &() { if(!value) throw std::runtime_error("Target is not set"); return *value; }
		Base &Get() { if(!value) throw std::runtime_error("Target is not set"); return *value; }

		virtual void Prepare(File &File);
	};

	//Font data
	class FontData : public IData {
	public:
		////03300C01h (Gaming, FontTheme, Data types, font)
		virtual int GetGID() const { return 0x03300C01; }
		Font value;
		FontInitiator initiator;

		FontData(const Font &font) { value=font; }
		FontData(const FontInitiator &font) { initiator=font; }
		operator Font() { return value; }
		virtual void Prepare(File &File) {
			value=(Font)initiator;
		}
	};

	////This is data resource which holds an array of basic data types. These types are
	/// integer, float, string, utils::Point and rectangle.
	class DataArray : public Base {
		friend DataArray *LoadDataResource(File &File, std::istream &Data, int Size);
	public:
		////Data collection
		utils::OrderedCollection<IData> Data;

		////Adds a new integer value to this resource
		IntegerData		&Add(int value);
		////Adds a new integer value to this resource
		ColorData		&Add(graphics::RGBint value);
		////Adds a new float value to this resource
		FloatData		&Add(float value);
		////Adds a new string value to this resource
		StringData		&Add(const std::string &value);
		////Adds a new utils::Point value to this resource
		PointData		&Add(utils::Point value);
		////Adds a new utils::Point2D value to this resource
		Point2DData		&Add(utils::Point2D value);
		////Adds a new utils::Size value to this resource
		SizeData		&Add(utils::Size value);
		////Adds a new rectangle to this resource
		RectangleData	&Add(utils::Rectangle value);
		////Adds a new bounds to this resource
		BoundsData		&Add(utils::Bounds value);
		////Adds a new margins to this resource
		MarginsData		&Add(utils::Margins value);
		////Adds a new rectangle to this resource
		FontData		&Add(Font value);
		////Adds a new rectangle to this resource
		FontData		&Add(FontInitiator value);
		////Adds a new integer value to this resource
		LinkData		&Add(utils::SGuid value);

		////Returns item at index
		IData *operator [] (int Index) { return &Data[Index]; }
		////Returns integer at index
		int getInt(int Index) { return dynamic_cast<IntegerData&>(Data[Index]).value; }
		////Returns integer at index
		gge::graphics::RGBint getColor(int Index) { 
			if(Data[Index].GetGID()==gge::resource::GID::Data_Color) {
				return dynamic_cast<ColorData&>(Data[Index]).value;
			}
			else {
				return dynamic_cast<IntegerData&>(Data[Index]).value;
			}
		}
		////Returns float at index
		float getFloat(int Index) { return dynamic_cast<FloatData&>(Data[Index]).value; }
		////Returns string at index
		std::string getString(int Index) { return dynamic_cast<StringData&>(Data[Index]).value; }
		////Returns char array at index
		const char *getText(int Index) { return dynamic_cast<StringData&>(Data[Index]).value.c_str(); }
		////Returns utils::Point at index
		utils::Point getPoint(int Index) { return dynamic_cast<PointData&>(Data[Index]).value; }
		////Returns utils::Point2D at index
		utils::Point getPoint2D(int Index) { return dynamic_cast<Point2DData&>(Data[Index]).value; }
		////Returns size at index
		utils::Size getSize(int Index) {
			if(Data[Index].GetGID()==gge::resource::GID::Data_Point) {
				return dynamic_cast<PointData&>(Data[Index]).value;
			}
			else {
				return dynamic_cast<SizeData&>(Data[Index]).value;
			}
		}
		////Returns rectangle at index
		utils::Rectangle getRectangle(int Index) { return dynamic_cast<RectangleData&>(Data[Index]).value; }
		////Returns utils::Margins at index
		utils::Margins getMargins(int Index) { 
			if(Data[Index].GetGID()==gge::resource::GID::Data_Rect) {
				return dynamic_cast<RectangleData&>(Data[Index]);
			} 
			else {
				return dynamic_cast<MarginsData&>(Data[Index]);
			}
		}
		////Returns utils::Bounds at index
		utils::Bounds getBounds(int Index) { 
			if(Data[Index].GetGID()==gge::resource::GID::Data_Rect) {
				return dynamic_cast<RectangleData&>(Data[Index]).value;
			} 
			else {
				return dynamic_cast<BoundsData&>(Data[Index]).value;
			}
		}
		LinkData &getLinkItem(int Index) { return dynamic_cast<LinkData&>(Data[Index]); }
		////Returns resource object from a link
		Base &getLink(int Index) { return dynamic_cast<LinkData&>(Data[Index]).Get(); }
		////Returns resource object from a link
		Base *getLinkPtr(int Index) { return dynamic_cast<LinkData&>(Data[Index]).value; }
		////Returns if the link is set and safe to read
		bool checkLink(int Index) { return dynamic_cast<LinkData&>(Data[Index]).value!=NULL; }
		////Returns font object
		Font getFont(int Index) { return dynamic_cast<FontData&>(Data[Index]).value; }
		////Returns number of items in the array
		int GetCount() { return Data.GetCount(); }
		
		////02030000h (Basic, Data resource)
		virtual GID::Type GetGID() const { return GID::Data; }

		virtual ~DataArray();

		virtual void Prepare(GGEMain &main, File &file);

	protected:
		File *file;
	};
} }
