#include "DataArray.h"
#include "File.h"

using namespace std;
using namespace gge::utils;

namespace gge { namespace resource {
	DataArray *LoadDataResource(File &File, std::istream &Data, int Size) {
		DataArray *dat=new DataArray;
		dat->file=&File;

		int target=Data.tellg()+Size;
		while(Data.tellg()<target) {
			int gid,size;

			ReadFrom(Data, gid);
			ReadFrom(Data, size);

			utils::SGuid tmpguid;

			char *tmpstr;

			if(gid==GID::Data_Text) {
				tmpstr=new char[size];
				Data.read(tmpstr,size);

				dat->Add(string(tmpstr,size));
                delete[] tmpstr;
			}
			else if(gid==GID::SGuid) {
				dat->guid.Load(Data);
			}
			else if(gid==GID::Guid) {
				dat->guid.LoadLong(Data);
			}
			else if(gid==GID::Data_Int) {
				dat->Add(ReadFrom<int>(Data));
			}
			else if(gid==GID::Data_Float) {
				dat->Add(ReadFrom<float>(Data));
			}
			else if(gid==GID::Data_Point) {
				dat->Add(ReadFrom<Point>(Data));
			}
			else if(gid==GID::Data_Point2D) {
				dat->Add(ReadFrom<Point2D>(Data));
			}
			else if(gid==GID::Data_Size) {
				dat->Add(ReadFrom<utils::Size>(Data));
			}
			else if(gid==GID::Data_Rect) {
				dat->Add(ReadFrom<Rectangle>(Data));
			}
			else if(gid==GID::Data_Bounds) {
				dat->Add(ReadFrom<Bounds>(Data));
			}
			else if(gid==GID::Data_Margins) {
				dat->Add(ReadFrom<Margins>(Data));
			}
			else if(gid==GID::Data_Link) {
				tmpguid.Load(Data);

				dat->Add(tmpguid);
			}
			else if(gid==GID::Data_Color) {
				dat->Add(ReadFrom<graphics::RGBint>(Data));
			}
			else if(gid==GID::Data_Font) {
				FontInitiator f;
				ReadFrom<int>(Data);//useless gid
				f=Font::Load(File, Data, ReadFrom<int>(Data));
				f.file=&File;

				dat->Add(f);
			}
			else ///!TODO: should query for pluggable data items
				EatChunk(Data,size);
		}

		return dat;
	}

	
	void LinkData::Prepare(File &File) { 
		value=File.Root().FindObject(guid);
	}

	template<class D_, class T_>
	D_ &AddTo(DataArray &d, const T_ &v) {
		D_ &o=*new D_(v);
		d.Data.Add(o);
		return o;
	}

	IntegerData		& DataArray::Add( int value ) {
		return AddTo<IntegerData>(*this, value);
	}

	ColorData		& DataArray::Add( graphics::RGBint value ) {
		return AddTo<ColorData>(*this, value);
	}

	FloatData		& DataArray::Add( float value ) {
		return AddTo<FloatData>(*this, value);
	}

	StringData		& DataArray::Add( const string &value ) {
		return AddTo<StringData>(*this, value);
	}

	PointData		& DataArray::Add( utils::Point value ) {
		return AddTo<PointData>(*this, value);
	}

	Point2DData		& DataArray::Add( utils::Point2D value ) {
		return AddTo<Point2DData>(*this, value);
	}

	SizeData		& DataArray::Add( utils::Size value ) {
		return AddTo<SizeData>(*this, value);
	}

	RectangleData	& DataArray::Add( utils::Rectangle value ) {
		return AddTo<RectangleData>(*this, value);
	}

	BoundsData	& DataArray::Add( utils::Bounds value ) {
		return AddTo<BoundsData>(*this, value);
	}

	MarginsData	& DataArray::Add( utils::Margins value ) {
		return AddTo<MarginsData>(*this, value);
	}

	FontData		& DataArray::Add( Font value ) {
		return AddTo<FontData>(*this, value);
	}

	FontData		& DataArray::Add( FontInitiator value ) {
		return AddTo<FontData>(*this, value);
	}

	LinkData		& DataArray::Add( utils::SGuid value ) {
		return AddTo<LinkData>(*this, value);
	}

	void DataArray::Prepare( GGEMain &main, File &file ) {
		for(utils::Collection<IData>::Iterator i=Data.First();i.IsValid();i.Next()) 
			i->Prepare(file);
	}

	DataArray::~DataArray() {
		Data.Destroy();
	}

} }

