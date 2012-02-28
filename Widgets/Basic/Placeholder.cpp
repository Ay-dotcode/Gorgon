#include "Placeholder.h"
#include "..\..\Resource\File.h"


using namespace gge::resource;
using namespace gge::utils;

namespace gge { namespace widgets {


	PlaceholderResource *LoadPlaceholderResource(resource::File& File, std::istream &Data, int Size) {
		PlaceholderResource *pr=new PlaceholderResource;


		int target=Data.tellg()+Size;
		while(Data.tellg()<target) {
			int gid,size;
			ReadFrom(Data, gid);
			ReadFrom(Data, size);

			if(gid==resource::GID::Guid) {
				pr->guid.LoadLong(Data);
			}
			else if(gid==resource::GID::SGuid) {
				pr->guid.Load(Data);
			}
			else if(gid==GID::Placeholder_Props) {
				pr->Align=(Alignment::Type)ReadFrom<int>(Data);
				ReadFrom(Data, pr->Margins);
				pr->SizingMode=(Placeholder::PlaceholderSizingMode)ReadFrom<int>(Data);
				ReadFrom(Data, pr->Minimum);
			}
			else {
				EatChunk(Data, size);
			}
		}

		return pr;
	}


}}