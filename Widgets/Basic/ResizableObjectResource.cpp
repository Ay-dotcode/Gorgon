
#include "Line.h"
#include "Rectangle.h"
#include "..\..\Resource\ResourceFile.h"
#include "..\..\Engine\Animation.h"
#include "..\..\Resource\AnimationResource.h"
#include "..\..\Resource\ImageResource.h"
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
				ResourceBase *rb=File.LoadObject(Data, gid, size);
				if(rb)
					rol->Subitems.Add(rb);
			}
		}

		rol->file = &File;
		return rol;
	}


	void ResizableObjectResource::Prepare(GGEMain &main) {
		ResourceBase::Prepare(main);

		resource::ResizableObjectProvider *provider= dynamic_cast<resource::ResizableObjectProvider *>(file->Root().FindObject(target));

		if(provider==NULL)
			provider=&resource::NullImage::Get();

		object=provider;
	}

}}
