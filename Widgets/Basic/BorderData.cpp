#include "BorderData.h"

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

	BorderDataResource *LoadBorderDataResource(resource::File& File, std::istream &Data, int Size) {
		BorderDataResource *bdr=new BorderDataResource;

		int target=Data.tellg()+Size;
		while(Data.tellg()<target) {
			int gid,size;
			ReadFrom(Data, gid);
			ReadFrom(Data, size);

			if(gid==resource::GID::Guid) {
				bdr->guid.LoadLong(Data);
			}
			else if(gid==resource::GID::SGuid) {
				bdr->guid.Load(Data);
			}
			else if(gid==GID::BorderData_Props) {
				bdr->target.Load(Data);
				ReadFrom(Data, bdr->Margins);
				ReadFrom(Data, bdr->Padding);
				ReadFrom(Data, bdr->BorderWidth);
				bdr->AutoBorderWidth = ReadFrom<int>(Data)!=0;
			}
			else {
				ResourceBase *rb=LoadResizableObjectResource(File, Data, size);

				if(rb)
					bdr->Subitems.Add(rb);
			}
		}

		return bdr;
	}

	void BorderDataResource::Prepare(GGEMain &main, resource::File &file) {
		ResourceBase::Prepare(main, file);

		ResizableObjectResource *ror= dynamic_cast<ResizableObjectResource *>(file.Root().FindObject(target));

		if(ror==NULL)
			ror=new ResizableObjectResource(NullImage::Get(), SizeController2D());

		object=ror;
	}

}}