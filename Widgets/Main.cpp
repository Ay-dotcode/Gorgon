
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

namespace gge { namespace widgets {

	VirtualPanel TopLevel;
	utils::Collection<WidgetBase> DrawQueue;


	void Draw_Signal(IntervalObject *interval, void *data) {
		for(utils::Collection<WidgetBase>::Iterator i=DrawQueue.First();i.isValid();i.Next()) {
			i->waitingforredraw=false;
			i->draw();
		}

		DrawQueue.Clear();
	}


	void RegisterLoaders(resource::File &File) {
		File.AddGameLoaders();
		File.Loaders.Add(new resource::ResourceLoader(GID::Line, LoadLineResource)); 
		File.Loaders.Add(new resource::ResourceLoader(GID::Rectangle, LoadRectangleResource)); 
		File.Loaders.Add(new resource::ResourceLoader(GID::ResizableObj, LoadResizableObjectResource)); 
		File.Loaders.Add(new resource::ResourceLoader(GID::BorderData, LoadBorderDataResource)); 
		File.Loaders.Add(new resource::ResourceLoader(GID::Placeholder, LoadPlaceholderResource)); 

		File.Loaders.Add(new resource::ResourceLoader(GID::Checkbox, checkbox::Load)); 
		File.Loaders.Add(new resource::ResourceLoader(GID::Slider, slider::Load)); 
		File.Loaders.Add(new resource::ResourceLoader(GID::Panel, panel::Load)); 
	}

	void Init(GGEMain &Main) {
		LayerBase *layer;
		layer=new LayerBase();

		Main.Add(layer, 1);
		TopLevel.LandOn(*layer);

		Main.RegisterInterval(1, NULL, &Draw_Signal);
	}

}}
