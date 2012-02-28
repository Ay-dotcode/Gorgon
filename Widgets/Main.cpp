
#include "Main.h"
#include "Definitions.h"
#include "Basic\Line.h"
#include "Basic\Rectangle.h"
#include "Basic\ResizableObjectResource.h"
#include "Basic\BorderData.h"
#include "Basic\Placeholder.h"

#include "Checkbox\CheckboxBlueprint.h"
#include "Slider\SliderBlueprint.h"
#include "Panel\PanelBlueprint.h"
#include "Textbox\TextboxBlueprint.h"
#include "Listbox\ListboxBlueprint.h"
#include "WidgetRegistry.h"
#include "..\Engine\Input.h"
#include "Combobox\ComboboxBlueprint.h"
#include "TabPanel.h"

namespace gge { namespace widgets {

	VirtualPanel TopLevel(0);
	utils::Collection<WidgetBase> DrawQueue;


	void Draw_Signal(IntervalObject &interval, void *data) {
		for(utils::Collection<WidgetBase>::Iterator i=DrawQueue.First();i.IsValid();i.Next()) {
			i->waitingforredraw=false;
			i->draw();
		}

		DrawQueue.Clear();
	}


	void RegisterLoaders(resource::File &File) {
		File.AddGameLoaders();
		File.Loaders.Add(new resource::Loader(GID::Line, LoadLineResource)); 
		File.Loaders.Add(new resource::Loader(GID::Rectangle, LoadRectangleResource)); 
		File.Loaders.Add(new resource::Loader(GID::ResizableObj, LoadResizableObjectResource)); 
		File.Loaders.Add(new resource::Loader(GID::BorderData, LoadBorderDataResource)); 
		File.Loaders.Add(new resource::Loader(GID::Placeholder, LoadPlaceholderResource)); 

		File.Loaders.Add(new resource::Loader(GID::Checkbox, checkbox::Load)); 
		File.Loaders.Add(new resource::Loader(GID::Slider, slider::Load)); 
		File.Loaders.Add(new resource::Loader(GID::Panel, panel::Load)); 
		File.Loaders.Add(new resource::Loader(GID::Textbox, textbox::Load)); 
		File.Loaders.Add(new resource::Loader(GID::Listbox, listbox::Load)); 
		File.Loaders.Add(new resource::Loader(GID::Combobox, combobox::Load)); 
		File.Loaders.Add(new resource::Loader(GID::Tabpanel, tabpanel::Load)); 

		File.Loaders.Add(new resource::Loader(GID::WR, LoadWR)); 
	}

	void Init(GGEMain &Main) {
		LayerBase *layer;
		layer=new LayerBase();

		Main.Add(layer, 1);
		TopLevel.init();
		TopLevel.LandOn(*layer);

		Main.RegisterInterval(1, &Draw_Signal);

		input::keyboard::Events.SetOrder(TopLevel.KeyboardToken,-1);
	}

}}
