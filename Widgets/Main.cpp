
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
	gge::resource::File *WidgetFile=NULL;
	graphics::Basic2DLayer *bglayer=NULL;


	void Draw_Signal(IntervalObject &interval) {
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

	void Initialize(GGEMain &Main, int TopLevelOrder) {
		LayerBase *layer;
		layer=new LayerBase();

		Main.Add(layer, TopLevelOrder);
		TopLevel.init();
		TopLevel.LandOn(*layer);
		 
		Main.RegisterInterval(1, &Draw_Signal);

		bglayer=new graphics::Basic2DLayer;
		Main.Add(bglayer, 10);
		Main.ResizeEvent.RegisterLambda([&] {
			if(WR.Pictures.Exists("background"))
				WR.Pictures("background").DrawIn(bglayer);
			TopLevel.Resize(gge::Main.BoundingBox.GetSize());
		});

		input::keyboard::Events.SetOrder(TopLevel.KeyboardToken,-1);
	}

	gge::resource::File & loadwidgets(const std::string &filename, bool prepare=true) {
		utils::CheckAndDelete(WidgetFile);
		WidgetFile=new gge::resource::File;
		RegisterLoaders(WidgetFile);
		WidgetFile->LoadFile(filename);

		if(prepare)
			WidgetFile->Prepare();


		return *WidgetFile;
	}

	gge::resource::File & LoadWidgets(const std::string &filename) {
		return loadwidgets(filename, true);
	}


	gge::resource::File & loadwidgets(bool prepare=true) {
		std::string filename;
		utils::CheckAndDelete(WidgetFile);
		WidgetFile=new gge::resource::File;
		RegisterLoaders(WidgetFile);

		std::string theme="";
		if(os::filesystem::IsFileExists(os::user::GetDocumentsPath()+"/theme.setting")) {
			std::ifstream themefile(os::user::GetDocumentsPath()+"theme.setting");
			std::getline(themefile, theme);
		}

		std::vector<std::string> filenamelist;
		filenamelist.push_back(theme+".ui.wgt");
		filenamelist.push_back("ui.wgt");
		filenamelist.push_back(os::user::GetDocumentsPath()+"/Gorgon/"+theme+".ui-"+UIFileRevision+".wgt");
		filenamelist.push_back(os::GetAppDataPath()+"/Gorgon/"+theme+".ui-"+UIFileRevision+".wgt");
		filenamelist.push_back("default.ui-"+UIFileRevision+".wgt");
		filenamelist.push_back(os::user::GetDocumentsPath()+"/Gorgon/default.ui-"+UIFileRevision+".wgt");
		filenamelist.push_back(os::GetAppDataPath()+"/Gorgon/default.ui-"+UIFileRevision+".wgt");

		filenamelist.push_back(os::user::GetDocumentsPath()+"/Gorgon/"+theme+".ui.wgt");
		filenamelist.push_back(os::GetAppDataPath()+"/Gorgon/"+theme+".ui.wgt");
		filenamelist.push_back("default.ui.wgt");
		filenamelist.push_back(os::user::GetDocumentsPath()+"/Gorgon/default.ui.wgt");
		filenamelist.push_back(os::GetAppDataPath()+"/Gorgon/default.ui.wgt");

		for(auto it=filenamelist.begin();it!=filenamelist.end();++it) {
			if(os::filesystem::IsFileExists(*it) || os::filesystem::IsFileExists(*it+".lzma")) {
				filename=*it;
				break;
			}
		}

		if(filename=="") {
			throw std::runtime_error("Cannot find UI file");
		}

		WidgetFile->LoadFile(filename);
		if(prepare)
			WidgetFile->Prepare();

		return *WidgetFile;
	}

	gge::resource::File & LoadWidgets() {
		return loadwidgets(true);
	}

	void InitializeApplication(const std::string &systemname, const std::string &windowtitle, const std::string &uifile, int width, int height, os::IconHandle icon) {
		loadwidgets(uifile, false);
		Main.Setup(systemname, width, height);
		Main.InitializeAll(windowtitle, icon);
		Initialize(Main);

		WidgetFile->Prepare();

		if(WR.Pictures.Exists("background"))
			WR.Pictures("background").DrawIn(bglayer);
	}

	void InitializeApplication(const std::string &systemname, const std::string &windowtitle, int width, int height, os::IconHandle icon) {
		loadwidgets(false);
		Main.Setup(systemname, width, height);
		Main.InitializeAll(windowtitle, icon);
		Initialize(Main);
		WidgetFile->Prepare();

		if(WR.Pictures.Exists("background"))
			WR.Pictures("background").DrawIn(bglayer);
	}


}}
