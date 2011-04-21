#include "ResizableObjectResource.h"
#include "Line.h"
#include "Rectangle.h"
#include <stdexcept>

using namespace gge::resource;

using std::runtime_error;


namespace gge { namespace widgets {
	ResourceBase *LoadResizableObject(ResourceFile* File, FILE* Data, int Size) {
		ResizableObjectResource *r=new ResizableObjectResource();
		r->file=File;

		int tpos=ftell(Data)+Size;

		int tmp;

		while(ftell(Data)<tpos) {
			int gid,size;
			fread(&gid,1,4,Data);
			fread(&size,1,4,Data);

			if(gid==GID_GUID) {
				r->guid=new Guid(Data);
			}
			if(gid==GID_RESIZABLEOBJECT_PROPS) {
				r->guid_target	= new Guid(Data);
				fread(&r->HTile, 4,1, Data);
				fread(&r->VTile, 4,1, Data);

				fread(&tmp, 4,1, Data);
				r->HIntegral.Type=(ResizableObject::IntegralSizeOption)tmp;
				fread(&tmp, 4,1, Data);
				r->VIntegral.Type=(ResizableObject::IntegralSizeOption)tmp;

				fread(&tmp, 4,1, Data);
				r->HIntegral.Overhead=(ResizableObject::IntegralSizeOption)tmp;
				fread(&tmp, 4,1, Data);
				r->VIntegral.Overhead=(ResizableObject::IntegralSizeOption)tmp;

				fread(&tmp, 4,1, Data);
				r->HIntegral.Increment=(ResizableObject::IntegralSizeOption)tmp;
				fread(&tmp, 4,1, Data);
				r->VIntegral.Increment=(ResizableObject::IntegralSizeOption)tmp;

				if(size!=(1 * 16 + 8 * 4))
					fseek(Data,size-(1 * 16 + 8 * 4),SEEK_CUR);
			} 

		}

		return r;
	}

	void ResizableObjectResource::Prepare(GGEMain *main) {
		target=file->Root().FindObject(guid_target);
	}

	ResizableObjectResource::operator ResizableObject *(){
		if(target) {
			switch(target->getGID()) {
				case GID_ANIMATION:
					return new ImageAnimation(dynamic_cast<AnimationResource*>(target), HTile, VTile);

				case GID_IMAGE:
					return new ImageResource(*dynamic_cast<ImageResource*>(target), HTile, VTile);

				case GID_LINE:
					if(dynamic_cast<LineResource*>(target)->isVertical)
						return new Line(dynamic_cast<LineResource*>(target), HTile, VIntegral);
					else
						return new Line(dynamic_cast<LineResource*>(target), VTile, HIntegral);

				case GID_RECT:
					return new ResizableRect(dynamic_cast<RectangleResource*>(target), HIntegral, VIntegral);

				default:
					return NULL;
			}
		} else
			return NULL;
	}

	ResizableObject &ResizableObjectResource::Generate (){
		if(target) {
			switch(target->getGID()) {
				case GID_ANIMATION:
					return *new ImageAnimation(dynamic_cast<AnimationResource*>(target), HTile, VTile);

				case GID_IMAGE:
					return *new ImageResource(*dynamic_cast<ImageResource*>(target), HTile, VTile);

				case GID_LINE:
					if(dynamic_cast<LineResource*>(target)->isVertical)
						return *new Line(dynamic_cast<LineResource*>(target), HTile, VIntegral);
					else
						return *new Line(dynamic_cast<LineResource*>(target), VTile, HIntegral);

				case GID_RECT:
					return *new ResizableRect(dynamic_cast<RectangleResource*>(target), HIntegral, VIntegral);

				default:
					throw runtime_error("Resizable object resource is not set");
			}
		} else
			throw std::runtime_error("target not set");
	}
} }
