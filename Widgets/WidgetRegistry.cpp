#include "WidgetRegistry.h"
#include "../Resource/DataResource.h"
#include "../Resource/LinkNode.h"

using namespace gge::resource;
using namespace gge::utils;
using namespace gge::animation;
using namespace std;

namespace gge { namespace widgets {


	void WidgetRegistryResource::Prepare(GGEMain &main, File &file) {
		ResourceBase::Prepare(main, file);
		this->file=&file;
		
		WR.SetWRR(*this);
	}

	void WidgetRegistryResource::Resolve(File &file) {
		Get<FolderResource>(12).Resolve(file);
	}

	WidgetRegistryResource * LoadWR(resource::File& File, std::istream &Data, int Size) {
		WidgetRegistryResource *wr=new WidgetRegistryResource;
		
		int target=Data.tellg()+Size;
		while(Data.tellg()<target) {
			int gid,size;
			ReadFrom(Data, gid);
			ReadFrom(Data, size);

			if(gid==resource::GID::Guid) {
				wr->guid.LoadLong(Data);
			}
			else if(gid==resource::GID::SGuid) {
				wr->guid.Load(Data);
			}
			else if(gid==resource::GID::Folder) {
				ResourceBase *res=LoadFolderResource(File, Data, size, true);
				if(res)
					wr->Subitems.Add(res, wr->Subitems.HighestOrder()+1);
			}
			else {
				ResourceBase *res=File.LoadObject(Data, gid,size);
				if(res)
					wr->Subitems.Add(res, wr->Subitems.HighestOrder()+1);
			}
		}

		return wr;
	}


	void WidgetRegistry::SetWRR(WidgetRegistryResource &wrr) {
		Fonts.Normal=wrr.Get<DataResource>(0).getFont(0);
		Fonts.H1=wrr.Get<DataResource>(0).getFont(1);
		Fonts.H2=wrr.Get<DataResource>(0).getFont(2);
		Fonts.H3=wrr.Get<DataResource>(0).getFont(3);
		Fonts.Strong=wrr.Get<DataResource>(0).getFont(4);
		Fonts.Emphasize=wrr.Get<DataResource>(0).getFont(5);
		Fonts.Quote=wrr.Get<DataResource>(0).getFont(6);
		Fonts.Info=wrr.Get<DataResource>(0).getFont(7);
		Fonts.Small=wrr.Get<DataResource>(0).getFont(8);
		Fonts.Link=wrr.Get<DataResource>(0).getFont(9);
		Fonts.Required=wrr.Get<DataResource>(0).getFont(10);
		Fonts.Important=wrr.Get<DataResource>(0).getFont(11);
		Fonts.Error=wrr.Get<DataResource>(0).getFont(12);
		Fonts.Warning=wrr.Get<DataResource>(0).getFont(13);
		Fonts.Success=wrr.Get<DataResource>(0).getFont(14);
		Fonts.Tooltip=wrr.Get<DataResource>(0).getFont(15);
		Fonts.Fixed=wrr.Get<DataResource>(0).getFont(16);


		Colors.Text=wrr.Get<DataResource>(1).getInt(0);
		Colors.Forecolor=wrr.Get<DataResource>(1).getInt(1);
		Colors.Background=wrr.Get<DataResource>(1).getInt(2);
		Colors.Alternate=wrr.Get<DataResource>(1).getInt(3);
		Colors.AlternateBackground=wrr.Get<DataResource>(1).getInt(4);
		Colors.Highlight=wrr.Get<DataResource>(1).getInt(5);
		Colors.Light=wrr.Get<DataResource>(1).getInt(6);
		Colors.Dark=wrr.Get<DataResource>(1).getInt(7);
		Colors.Saturated=wrr.Get<DataResource>(1).getInt(8);
		Colors.Desaturated=wrr.Get<DataResource>(1).getInt(9);
		Colors.Shadow=wrr.Get<DataResource>(1).getInt(10);

		Buttons.Button=Button=dynamic_cast<checkbox::Blueprint*>(wrr.Get<FolderResource>(2).Get<LinkNodeResource>(0).GetTarget(*wrr.file));


		for(auto it=wrr.Get<FolderResource>(12).Subitems.First();it.isValid();it.Next()) {
			try {
				icons.insert(pair<string, RectangularGraphic2DSequenceProvider&>(it->name, dynamic_cast<RectangularGraphic2DSequenceProvider&>(*it)));
			}
			catch(...) { }//don't insert if it is not an animation
		}

		for(auto it=wrr.Get<FolderResource>(13).Subitems.First();it.isValid();it.Next()) {
			try {
				pictures.insert(pair<string, RectangularGraphic2DSequenceProvider&>(it->name, dynamic_cast<RectangularGraphic2DSequenceProvider&>(*it)));
			}
			catch(...) { }//don't insert if it is not an animation
		}
	}



	WidgetRegistry WR;

}}
