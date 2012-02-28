
#include "Line.h"
#include "Rectangle.h"
#include "..\..\Resource\File.h"
#include "..\..\Engine\Animation.h"
#include "..\..\Resource\Animation.h"
#include "..\..\Resource\Image.h"
#include "..\..\Resource\NullImage.h"
#include "..\..\Utils\SGuid.h"
#include "ResizableObjectResource.h"

using namespace gge::utils;
using namespace gge::resource;
using namespace gge::animation;
using namespace gge::graphics;
using namespace std;

namespace gge { namespace widgets {

	ResizableObjectResource *LoadResizableObjectResource(resource::File& File, std::istream &Data, int Size) {
		ResizableObjectResource *rol=new ResizableObjectResource;

		int target=Data.tellg()+Size;
		while(Data.tellg()<target) {
			int gid,size;
			ReadFrom(Data, gid);
			ReadFrom(Data, size);

			if(gid==resource::GID::Guid) {
				rol->guid.LoadLong(Data);
			}
			else if(gid==resource::GID::SGuid) {
				rol->guid.Load(Data);
			}
			else if(gid==GID::ResizableObj_Props) {
				rol->target.Load(Data);
				rol->SizeController.Align=(Alignment::Type)ReadFrom<int>(Data);
				rol->SizeController.HorizontalTiling=(SizeController2D::TilingType)ReadFrom<int>(Data);
				rol->SizeController.VerticalTiling=(SizeController2D::TilingType)ReadFrom<int>(Data);
			}
			else {
				resource::Base *rb=File.LoadObject(Data, gid, size);
				if(rb)
					rol->Subitems.Add(rb);
			}
		}

		return rol;
	}


	void ResizableObjectResource::Prepare(GGEMain &main, resource::File &file) {
		Base::Prepare(main, file);

		resource::ResizableObjectProvider *provider= dynamic_cast<resource::ResizableObjectProvider *>(file.Root().FindObject(target));

		if(provider==NULL)
			provider=&resource::NullImage::Get();

		object=provider;
	}

}}
