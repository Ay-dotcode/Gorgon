#include "WidgetRegistry.h"
#include "../Resource/DataArray.h"
#include "../Resource/LinkNode.h"

using namespace gge::resource;
using namespace gge::utils;
using namespace gge::animation;
using namespace std;

namespace gge { namespace widgets {


	void WidgetRegistryResource::Prepare(GGEMain &main, File &file) {
		Base::Prepare(main, file);
		this->file=&file;
		
		WR.SetWRR(*this);
	}

	void WidgetRegistryResource::Resolve(File &file) {
		Get<Folder>(15).Resolve(file);
		Get<Folder>(16).Resolve(file);
		Get<Folder>(17).Resolve(file);
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
				resource::Base *res=LoadFolderResource(File, Data, size, true);
				if(res)
					wr->Subitems.Add(res, wr->Subitems.HighestOrder()+1);
			}
			else {
				resource::Base *res=File.LoadObject(Data, gid,size);
				if(res)
					wr->Subitems.Add(res, wr->Subitems.HighestOrder()+1);
			}
		}

		return wr;
	}

	template<class T_>
	T_ *GetWithDefault(Folder *folder, File &file, int ind, T_ *def) {
		T_ *temp=NULL;
		if(folder->Subitems.GetCount()>ind) {
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
		int item=0;

		DataArray &infodata=wrr.Get<DataArray>(item);
		WidgetSpacing=infodata.getPoint(0);


		DataArray &fontdata=wrr.Get<DataArray>(++item);
		Fonts.Normal		=fontdata.getFont(0);
		Fonts.H1			=fontdata.getFont(1);
		Fonts.H2			=fontdata.getFont(2);
		Fonts.H3			=fontdata.getFont(3);
		Fonts.Strong		=fontdata.getFont(4);
		Fonts.Emphasize		=fontdata.getFont(5);
		Fonts.Quote			=fontdata.getFont(6);
		Fonts.Info			=fontdata.getFont(7);
		Fonts.Small			=fontdata.getFont(8);
		Fonts.Link			=fontdata.getFont(9);
		Fonts.Required		=fontdata.getFont(10);
		Fonts.Important		=fontdata.getFont(11);
		Fonts.Error			=fontdata.getFont(12);
		Fonts.Warning		=fontdata.getFont(13);
		Fonts.Success		=fontdata.getFont(14);
		Fonts.Tooltip		=fontdata.getFont(15);
		Fonts.Fixed			=fontdata.getFont(16);
		Fonts.Decorative	=fontdata.getFont(17);


		DataArray &colordata=wrr.Get<DataArray>(++item);
		Colors.Text					=colordata.getInt(0);
		Colors.Forecolor			=colordata.getInt(1);
		Colors.Background			=colordata.getInt(2);
		Colors.Alternate			=colordata.getInt(3);
		Colors.AlternateBackground	=colordata.getInt(4);
		Colors.Highlight			=colordata.getInt(5);
		Colors.Light				=colordata.getInt(6);
		Colors.Dark					=colordata.getInt(7);
		Colors.Saturated			=colordata.getInt(8);
		Colors.Desaturated			=colordata.getInt(9);
		Colors.Shadow				=colordata.getInt(10);


		Folder *folder;

		Pointers.Fetch(wrr.Get<Folder>(++item));
		Pointers.Show();

		folder=&wrr.Get<Folder>(++item);
		Buttons.Button=Button=dynamic_cast<checkbox::Blueprint*>(folder->Get<LinkNodeResource>(0).GetTarget(*wrr.file));
		Buttons.Dialog		= GetWithDefault(folder, *wrr.file, 1, Buttons.Button);
		Buttons.Menu		= GetWithDefault(folder, *wrr.file, 2, Buttons.Button);
		Buttons.Tool		= GetWithDefault(folder, *wrr.file, 3, Buttons.Button);
		Buttons.Navigation	= GetWithDefault(folder, *wrr.file, 4, Buttons.Dialog);
		Buttons.Large		= GetWithDefault(folder, *wrr.file, 5, Buttons.Button);
		Buttons.Small		= GetWithDefault(folder, *wrr.file, 6, Buttons.Button);
		Buttons.Symbol		= GetWithDefault(folder, *wrr.file, 7, Buttons.Tool);
		Buttons.Browse		= GetWithDefault(folder, *wrr.file, 8, Buttons.Symbol);


		folder=&wrr.Get<Folder>(++item);
		Textboxes.Textbox=Textbox=dynamic_cast<textbox::Blueprint*>(folder->Get<LinkNodeResource>(0).GetTarget(*wrr.file));
		Textboxes.Numberbox	= GetWithDefault(folder, *wrr.file, 1, Textboxes.Textbox);
		Textboxes.Small		= GetWithDefault(folder, *wrr.file, 2, Textboxes.Textbox);
		Textboxes.Password	= GetWithDefault(folder, *wrr.file, 3, Textboxes.Textbox);
		Textboxes.TextEdit	= GetWithDefault(folder, *wrr.file, 4, Textboxes.Textbox);
		Textboxes.CodeEdit	= GetWithDefault(folder, *wrr.file, 5, Textboxes.TextEdit);


		folder=&wrr.Get<Folder>(++item);
		Checkboxes.Checkbox=Checkbox=dynamic_cast<checkbox::Blueprint*>(folder->Get<LinkNodeResource>(0).GetTarget(*wrr.file));
		Checkboxes.Radio=RadioButton=dynamic_cast<checkbox::Blueprint*>(folder->Get<LinkNodeResource>(1).GetTarget(*wrr.file));
		Checkboxes.Toggle	= GetWithDefault(folder, *wrr.file, 2, Checkboxes.Checkbox);
		Checkboxes.More		= GetWithDefault(folder, *wrr.file, 3, Checkboxes.Checkbox);
		Checkboxes.MenuCheck= GetWithDefault(folder, *wrr.file, 4, Checkboxes.Checkbox);
		Checkboxes.MenuRadio= GetWithDefault(folder, *wrr.file, 5, Checkboxes.Radio);
		Checkboxes.Lock		= GetWithDefault(folder, *wrr.file, 6, Checkboxes.Checkbox);


		folder=&wrr.Get<Folder>(++item);
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


		folder=&wrr.Get<Folder>(++item);
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


		folder=&wrr.Get<Folder>(++item);
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


		folder=&wrr.Get<Folder>(++item);
		Progressbars.Progressbar=Progressbar=dynamic_cast<slider::Blueprint*>(folder->Get<LinkNodeResource>(0).GetTarget(*wrr.file));
		Progressbars.File		 = GetWithDefault(folder, *wrr.file, 1, Progressbars.Progressbar);
		Progressbars.Working	 = GetWithDefault(folder, *wrr.file, 2, Progressbars.Progressbar);
		Progressbars.Loading	 = GetWithDefault(folder, *wrr.file, 3, Progressbars.Progressbar);
		Progressbars.Download	 = GetWithDefault(folder, *wrr.file, 4, Progressbars.Progressbar);
		Progressbars.Percent	 = GetWithDefault(folder, *wrr.file, 6, Progressbars.Progressbar);

		Progressbars.Unknown=dynamic_cast<animation::RectangularGraphic2DAnimationProvider*>(folder->Get<LinkNodeResource>(5).GetTarget(*wrr.file));
		if(!Progressbars.Unknown)
			Progressbars.Unknown=&resource::NullImage::Get();

		folder=&wrr.Get<Folder>(++item);
		Panels.Panel=Panel=dynamic_cast<panel::Blueprint*>(folder->Get<LinkNodeResource>(0).GetTarget(*wrr.file));
		Panels.OverlayControls	= GetWithDefault(folder, *wrr.file, 1, Panels.Panel);
		Window=Panels.Window	= GetWithDefault(folder, *wrr.file, 2, Panels.Panel);
		Panels.DialogWindow		= GetWithDefault(folder, *wrr.file, 3, Panels.Window);
		Panels.Toolbar			= GetWithDefault(folder, *wrr.file, 4, Panels.Panel);
		Panels.Menubar			= GetWithDefault(folder, *wrr.file, 5, Panels.Toolbar);
		Panels.ToolWindow		= GetWithDefault(folder, *wrr.file, 6, Panels.Window);
		Panels.SettingsWindow	= GetWithDefault(folder, *wrr.file, 7, Panels.Window);
		Panels.Tabpanel=dynamic_cast<tabpanel::Blueprint*>(folder->Get<LinkNodeResource>(8).GetTarget(*wrr.file));
		Panels.Tooltab			= GetWithDefault(folder, *wrr.file, 9, Panels.Tabpanel);
		Panels.Settingstab		= GetWithDefault(folder, *wrr.file, 10, Panels.Tabpanel);


		folder=&wrr.Get<Folder>(++item);
		Others.Combobox=Combobox=dynamic_cast<combobox::Blueprint*>(folder->Get<LinkNodeResource>(0).GetTarget(*wrr.file));
		Others.Autocomplete	= GetWithDefault(folder, *wrr.file, 1, Others.Combobox);
		Others.JumpList	= GetWithDefault(folder, *wrr.file, 2, Others.Combobox);

		++item; //separators
		++item; //borders



		for(auto it=wrr.Get<Folder>(++item).Subitems.First();it.IsValid();it.Next()) {
			try {
				if(it->name=="") {
					icons.insert(pair<string, RectangularGraphic2DSequenceProvider&>(it->name, dynamic_cast<RectangularGraphic2DSequenceProvider&>(*it)));
					continue;
				}

				string name=it->name;
				string::size_type pos;
				do {
					pos=name.find_first_of(',');
					
					if(pos==string::npos)
						pos=name.length();
					
					string n=name.substr(0, pos);
					
					if(pos<name.length())
						name=name.substr(pos+1);
					else
						name="";

					if(n!="")
						icons.insert(pair<string, RectangularGraphic2DSequenceProvider&>(n, dynamic_cast<RectangularGraphic2DSequenceProvider&>(*it)));
				} while(name!="");
			}
			catch(...) { }//don't insert if it is not an animation
		}

		for(auto it=wrr.Get<Folder>(++item).Subitems.First();it.IsValid();it.Next()) {
			try {
				if(it->name=="") {
					pictures.insert(pair<string, RectangularGraphic2DSequenceProvider&>(it->name, dynamic_cast<RectangularGraphic2DSequenceProvider&>(*it)));
					continue;
				}

				string name=it->name;
				string::size_type pos;
				do {
					pos=name.find_first_of(',');

					if(pos==string::npos)
						pos=name.length();

					string n=name.substr(0, pos);

					if(pos<name.length())
						name=name.substr(pos+1);
					else
						name="";

					if(n!="")
						pictures.insert(pair<string, RectangularGraphic2DSequenceProvider&>(n, dynamic_cast<RectangularGraphic2DSequenceProvider&>(*it)));
				} while(name!="");
			}
			catch(...) { }//don't insert if it is not an animation
		}

		for(auto it=wrr.Get<Folder>(++item).Subitems.First();it.IsValid();it.Next()) {
			try {
				if(it->name=="") {
					sounds.insert(pair<string, Sound&>(it->name, dynamic_cast<Sound&>(*it)));
					continue;
				}

				string name=it->name;
				string::size_type pos;
				do {
					pos=name.find_first_of(',');

					if(pos==string::npos)
						pos=name.length();

					string n=name.substr(0, pos);

					if(pos<name.length())
						name=name.substr(pos+1);
					else
						name="";

					if(n!="")
						sounds.insert(pair<string, Sound&>(n, dynamic_cast<Sound&>(*it)));
				} while(name!="");
			}
			catch(...) { }//don't insert if it is not a sound
		}

		LoadedEvent();
	}



	WidgetRegistry WR;

}}
