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

	template<class T_>
	T_ *GetWithDefault(FolderResource *folder, File &file, int ind, T_ *def) {
		T_ *temp=NULL;
		if(folder->Subitems.getCount()>ind) {
			try {
				LinkNodeResource &lnk=folder->Get<LinkNodeResource>(ind);
				temp=dynamic_cast<T_*>(lnk.GetTarget(file));
			} 
			catch(...) { }
		}

		if(!temp)
			temp=def;

		return temp;
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
		Fonts.Decorative=wrr.Get<DataResource>(0).getFont(17);


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


		FolderResource *folder;

		Pointers.Fetch(wrr.Get<FolderResource>(2));
		Pointers.Show();

		folder=&wrr.Get<FolderResource>(3);
		Buttons.Button=Button=dynamic_cast<checkbox::Blueprint*>(folder->Get<LinkNodeResource>(0).GetTarget(*wrr.file));
		Buttons.Dialog		= GetWithDefault(folder, *wrr.file, 1, Buttons.Button);
		Buttons.Menu		= GetWithDefault(folder, *wrr.file, 2, Buttons.Button);
		Buttons.Tool		= GetWithDefault(folder, *wrr.file, 3, Buttons.Button);
		Buttons.Navigation	= GetWithDefault(folder, *wrr.file, 4, Buttons.Dialog);
		Buttons.Large		= GetWithDefault(folder, *wrr.file, 5, Buttons.Button);
		Buttons.Small		= GetWithDefault(folder, *wrr.file, 6, Buttons.Button);
		Buttons.Symbol		= GetWithDefault(folder, *wrr.file, 7, Buttons.Tool);
		Buttons.Browse		= GetWithDefault(folder, *wrr.file, 8, Buttons.Symbol);


		folder=&wrr.Get<FolderResource>(4);
		Textboxes.Textbox=Textbox=dynamic_cast<textbox::Blueprint*>(folder->Get<LinkNodeResource>(0).GetTarget(*wrr.file));
		Textboxes.Numberbox	= GetWithDefault(folder, *wrr.file, 1, Textboxes.Textbox);
		Textboxes.Small		= GetWithDefault(folder, *wrr.file, 2, Textboxes.Textbox);
		Textboxes.Password	= GetWithDefault(folder, *wrr.file, 3, Textboxes.Textbox);
		Textboxes.TextEdit	= GetWithDefault(folder, *wrr.file, 4, Textboxes.Textbox);
		Textboxes.CodeEdit	= GetWithDefault(folder, *wrr.file, 5, Textboxes.TextEdit);


		folder=&wrr.Get<FolderResource>(5);
		Checkboxes.Checkbox=Checkbox=dynamic_cast<checkbox::Blueprint*>(folder->Get<LinkNodeResource>(0).GetTarget(*wrr.file));
		Checkboxes.Radio=RadioButton=dynamic_cast<checkbox::Blueprint*>(folder->Get<LinkNodeResource>(1).GetTarget(*wrr.file));
		Checkboxes.Toggle	= GetWithDefault(folder, *wrr.file, 2, Checkboxes.Checkbox);
		Checkboxes.More		= GetWithDefault(folder, *wrr.file, 3, Checkboxes.Checkbox);
		Checkboxes.MenuCheck= GetWithDefault(folder, *wrr.file, 4, Checkboxes.Checkbox);
		Checkboxes.MenuRadio= GetWithDefault(folder, *wrr.file, 5, Checkboxes.Radio);
		Checkboxes.Lock		= GetWithDefault(folder, *wrr.file, 6, Checkboxes.Checkbox);


		folder=&wrr.Get<FolderResource>(6);
		Labels.Label=Label=dynamic_cast<checkbox::Blueprint*>(folder->Get<LinkNodeResource>(0).GetTarget(*wrr.file));
		Labels.Title		= GetWithDefault(folder, *wrr.file, 1, Labels.Label);
		Labels.DataCaption	= GetWithDefault(folder, *wrr.file, 2, Labels.Title);
		Labels.Bold			= GetWithDefault(folder, *wrr.file, 3, Labels.Title);
		Labels.Heading		= GetWithDefault(folder, *wrr.file, 4, Labels.Bold);
		Labels.Subheading	= GetWithDefault(folder, *wrr.file, 5, Labels.Heading);
		Labels.ListCaption	= GetWithDefault(folder, *wrr.file, 6, Labels.Title);
		Labels.Required		= GetWithDefault(folder, *wrr.file, 7, Labels.Label);
		Labels.Hint			= GetWithDefault(folder, *wrr.file, 8, Labels.Label);
		Labels.Tooltip		= GetWithDefault(folder, *wrr.file, 9, Labels.Label);
		Labels.Link			= GetWithDefault(folder, *wrr.file, 10, Labels.Label);


		folder=&wrr.Get<FolderResource>(7);
		Listboxes.Listbox=Listbox=dynamic_cast<listbox::Blueprint*>(folder->Get<LinkNodeResource>(0).GetTarget(*wrr.file));
		Listboxes.Action	= GetWithDefault(folder, *wrr.file, 1, Listboxes.Listbox);
		Listboxes.Picture	= GetWithDefault(folder, *wrr.file, 2, Listboxes.Listbox);
		Listboxes.File		= GetWithDefault(folder, *wrr.file, 3, Listboxes.Listbox);
		Listboxes.FileView	= GetWithDefault(folder, *wrr.file, 4, Listboxes.Listbox);
		Listboxes.Folder	= GetWithDefault(folder, *wrr.file, 5, Listboxes.File);
		Listboxes.DataTable	= GetWithDefault(folder, *wrr.file, 6, Listboxes.Listbox);
		Listboxes.Detailed	= GetWithDefault(folder, *wrr.file, 7, Listboxes.DataTable);
		Listboxes.Checklist	= GetWithDefault(folder, *wrr.file, 8, Listboxes.Listbox);
		Listboxes.Radiolist	= GetWithDefault(folder, *wrr.file, 9, Listboxes.Checklist);
		Listboxes.Edit		= GetWithDefault(folder, *wrr.file, 10, Listboxes.Listbox);


		folder=&wrr.Get<FolderResource>(8);
		Sliders.Slider=Slider=dynamic_cast<slider::Blueprint*>(folder->Get<LinkNodeResource>(0).GetTarget(*wrr.file));
		Sliders.Selectbar	 = GetWithDefault(folder, *wrr.file, 2, Sliders.Slider);
		Sliders.ImageSelector= GetWithDefault(folder, *wrr.file, 3, Sliders.Selectbar);
		Sliders.Named		 = GetWithDefault(folder, *wrr.file, 4, Sliders.Selectbar);
		Sliders.Radiobar	 = GetWithDefault(folder, *wrr.file, 5, Sliders.Selectbar);
		Sliders.NumberSpinner= GetWithDefault(folder, *wrr.file, 8, Sliders.Slider);
		Sliders.ItemSpinner	 = GetWithDefault(folder, *wrr.file, 9, Sliders.NumberSpinner);

		Scrollbars.Scrollbar=Scrollbar=dynamic_cast<slider::Blueprint*>(folder->Get<LinkNodeResource>(1).GetTarget(*wrr.file));
		Scrollbars.Dialog	= GetWithDefault(folder, *wrr.file, 6, Scrollbars.Scrollbar);
		Scrollbars.Picture	= GetWithDefault(folder, *wrr.file, 7, Scrollbars.Scrollbar);


		folder=&wrr.Get<FolderResource>(9);
		Progressbars.Progressbar=Progressbar=dynamic_cast<slider::Blueprint*>(folder->Get<LinkNodeResource>(0).GetTarget(*wrr.file));
		Progressbars.File		 = GetWithDefault(folder, *wrr.file, 1, Progressbars.Progressbar);
		Progressbars.Working	 = GetWithDefault(folder, *wrr.file, 2, Progressbars.Progressbar);
		Progressbars.Loading	 = GetWithDefault(folder, *wrr.file, 3, Progressbars.Progressbar);
		Progressbars.Download	 = GetWithDefault(folder, *wrr.file, 4, Progressbars.Progressbar);
		Progressbars.Percent	 = GetWithDefault(folder, *wrr.file, 6, Progressbars.Progressbar);

		Progressbars.Unknown=dynamic_cast<animation::RectangularGraphic2DAnimationProvider*>(folder->Get<LinkNodeResource>(5).GetTarget(*wrr.file));
		if(!Progressbars.Unknown)
			Progressbars.Unknown=&resource::NullImage::Get();

		folder=&wrr.Get<FolderResource>(10);
		Panels.Panel=Panel=dynamic_cast<panel::Blueprint*>(folder->Get<LinkNodeResource>(0).GetTarget(*wrr.file));
		Panels.OverlayControls	= GetWithDefault(folder, *wrr.file, 1, Panels.Panel);
		Panels.Window			= GetWithDefault(folder, *wrr.file, 2, Panels.Panel);
		Panels.DialogWindow		= GetWithDefault(folder, *wrr.file, 3, Panels.Window);
		Panels.Toolbar			= GetWithDefault(folder, *wrr.file, 4, Panels.Panel);
		Panels.Menubar			= GetWithDefault(folder, *wrr.file, 5, Panels.Toolbar);
		Panels.ToolWindow		= GetWithDefault(folder, *wrr.file, 6, Panels.Window);
		Panels.SettingsWindow	= GetWithDefault(folder, *wrr.file, 7, Panels.Window);


		for(auto it=wrr.Get<FolderResource>(14).Subitems.First();it.isValid();it.Next()) {
			try {
				icons.insert(pair<string, RectangularGraphic2DSequenceProvider&>(it->name, dynamic_cast<RectangularGraphic2DSequenceProvider&>(*it)));
			}
			catch(...) { }//don't insert if it is not an animation
		}

		for(auto it=wrr.Get<FolderResource>(15).Subitems.First();it.isValid();it.Next()) {
			try {
				pictures.insert(pair<string, RectangularGraphic2DSequenceProvider&>(it->name, dynamic_cast<RectangularGraphic2DSequenceProvider&>(*it)));
			}
			catch(...) { }//don't insert if it is not an animation
		}

		for(auto it=wrr.Get<FolderResource>(16).Subitems.First();it.isValid();it.Next()) {
			try {
				sounds.insert(pair<string, SoundResource&>(it->name, dynamic_cast<SoundResource&>(*it)));
			}
			catch(...) { }//don't insert if it is not an animation
		}
	}



	WidgetRegistry WR;

}}
