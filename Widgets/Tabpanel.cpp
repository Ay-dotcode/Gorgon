#include "Tabpanel.h"
#include "../Resource/File.h"

using namespace gge::resource;
using namespace gge::utils;

namespace gge { namespace widgets {

	namespace tabpanel {


		Blueprint * Load(resource::File& File, std::istream &Data, int Size) {

			Blueprint *bp=new Blueprint;

			int target=Data.tellg()+Size;
			while(Data.tellg()<target) {
				int gid,size;
				ReadFrom(Data, gid);
				ReadFrom(Data, size);

				if(gid==resource::GID::Guid) {
					bp->guid.LoadLong(Data);
				}
				else if(gid==resource::GID::SGuid) {
					bp->guid.Load(Data);
				}
				else if(gid==GID::Tabpanel_Props) {

					ReadFrom(Data, bp->DefaultSize);
					bp->panel.Load(Data);
					bp->radio.Load(Data);
					bp->placeholder.Load(Data);


					EatChunk(Data, size-(1 * 8 + 3 * 8));
				}
				else {
					resource::Base *res=File.LoadObject(Data, gid,size);
					if(res)
						bp->Subitems.Add(res, bp->Subitems.HighestOrder()+1);
				}
			}


			return bp;
		}

		void Blueprint::Prepare(GGEMain &main, resource::File &file) {
			Base::Prepare(main, file);

			file.FindObject(panel, Panel);
			file.FindObject(radio, Radio);
			widgets::Placeholder *p;
			file.FindObject(placeholder, p);
			if(p) {
				utils::CheckAndDelete(Placeholder);
				Placeholder=p;
			}
		}
	}



	bool Tabpanel::Focus() {
		WidgetBase::Focus();

		for(auto it=First();it.IsValid();it.Next()) {
			if(it->IsVisible())
				return it->Focus();
		}

		return false;
	}

	void Tabpanel::Disable() {
		WidgetBase::Disable();
		
		controls.InformEnabledChange(false);
	}

	void Tabpanel::Enable() {
		WidgetBase::Enable();
		
		controls.InformEnabledChange(true);
	}

	void Tabpanel::Resize(utils::Size Size) {
		WidgetBase::Resize(Size);
		if(Size.Width==0 && bp)
			Size.Width=bp->DefaultSize.Width;
		if(Size.Height==0 && bp)
			Size.Height=bp->DefaultSize.Height;

		if(BaseLayer)
			BaseLayer->Resize(Size);
		controls.Resize(Size);
		reorganize();
	}

	bool Tabpanel::KeyboardHandler(input::keyboard::Event::Type event, input::keyboard::Key Key) {
		return controls.KeyboardEvent(input::keyboard::Event(event, Key));
	}

	bool Tabpanel::loosefocus(bool force) {
		if(force)
			controls.ForceRemoveFocus();
		else
			return controls.RemoveFocus();

		return true;
	}

	void Tabpanel::reorganize() {
		auto btn=buttons.First();
		int x=0;
		int y=0;
		bool newbutton=false;

		if(!bp)
			return;

		if(bp->Placeholder->SizingMode==bp->Placeholder->Contents || bp->Placeholder->SizingMode==bp->Placeholder->Free || bp->Placeholder->Minimum.Height==0) {
			if(btn.IsValid()) {
				y=btn->GetHeight();
			}
		}
		else {
			y=bp->Placeholder->Minimum.Height;
		}

		y+=bp->Placeholder->Margins.TotalY();


		for(auto it=First();it.IsValid();it.Next()) {
			if(!it->IsEnabled() && it.CurrentPtr()==active) {
				ActivateAnother(*it);
				reorganize();
				return;
			}
			if(it->IsVisible()) {
				it->SetY(y);
				it->SetWidth(size.Width);
				it->SetHeight(size.Height-y);
				it->SetZOrder(2);

				if(btn.IsValid()) {
					RadioButton<tabpanel::Panel*> &rad=*btn;

					rad.Text=it->Title;
					rad.Move(x,bp->Placeholder->Margins.Top);
					rad.Value=it.CurrentPtr();
					rad.SetEnabled(it->IsEnabled());
					rad.SetZOrder(1);
					x+=rad.GetWidth();
					rad.Show();

					if(it.CurrentPtr()==active)
						rad.Check();
					else
						rad.Uncheck();

					btn.Next();
				}
				else {
					RadioButton<tabpanel::Panel*> &rad=*new RadioButton<tabpanel::Panel*>;

					newbutton=true;
				
					rad.SetContainer(controls);
					if(bp)
						rad.SetBlueprint(bp->Radio);

					buttons.Add(rad);
					rad.Autosize=AutosizeModes::GrowOnly;
					rad.SetEnabled(it->IsEnabled());
					rad.Text=it->Title;
					rad.Move(x,bp->Placeholder->Margins.Top);
					rad.Value=it.CurrentPtr();
					rad.ChangeEvent.Register(this,&Tabpanel::tab_click);

					if(it.CurrentPtr()==active)
						rad.Check();
					else
						rad.Uncheck();

					x+=rad.GetWidth();
				}
			}
		}

		for(;btn.IsValid();btn.Next())
			btn->Hide();

		x=Alignment::CalculateLocation(bp->Placeholder->Align, Bounds(0,0,size.Width,size.Height),Size(x, y), bp->Placeholder->Margins).x;
		for(btn=buttons.First();btn.IsValid();btn.Next()) {
			btn->SetX(x);
			x+=btn->GetWidth();
		}

		if(y==bp->Placeholder->Margins.TotalY() && newbutton)
			reorganize();
	}

	void Tabpanel::tab_click(RadioButton<tabpanel::Panel*> &object) {
		Activate(object.Value, true);
	}

	void Tabpanel::Activate(tabpanel::Panel *panel, bool setfocus) {
		if(active!=panel) {
			for(auto it=First();it.IsValid();it.Next()) {
				it->setinactive(true);
			}

			if(panel)
				panel->setinactive(false);

			active=panel;

			for(auto it=buttons.First();it.IsValid();it.Next()) {
				if(it->Value==panel)
					it->Check();
				else
					it->Uncheck();
			}
		}
	}

	void Tabpanel::Remove(tabpanel::Panel &item) {
		if(&item==active) {
			ActivateAnother(item);
		}

		controls.RemoveWidget(item);
		reorganize();
		OrderedCollection::Remove(item);
	}

	void Tabpanel::setblueprint(const widgets::Blueprint & bp) {
		this->bp=static_cast<const tabpanel::Blueprint*>(&bp);
		if(this->bp) {
			for(auto it=First();it.IsValid();it.Next()) {
				it->SetBlueprint(this->bp->Panel);
			}

			for(auto it=controls.Widgets.First();it.IsValid();it.Next()) {
				if(dynamic_cast<RadioButton<tabpanel::Panel*> *>(it.CurrentPtr()))
					dynamic_cast<RadioButton<tabpanel::Panel*> &>(*it).SetBlueprint(this->bp->Radio);
			}
		}

		Resize(size);
	}

	void Tabpanel::ActivateAnother(tabpanel::Panel & item) {
		if(GetCount()==1) {
			Activate(NULL);
		}
		else {
			for(auto it=First();it.IsValid();it.Next()) {
				if(it.CurrentPtr()==&item) {
					it.Next();
					if(it.IsValid())
						Activate(*it);
					else
						Activate(*First());
				}
			}
		}
	}


}}
