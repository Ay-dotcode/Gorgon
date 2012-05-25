#pragma once

//This file is the main entry file of Widgets, 
// this contains the function that allows loading
// widget files
// 

#include "../Resource/File.h"
#include "../Engine/GGEMain.h"
#include "Base/VirtualPanel.h"


namespace gge { namespace widgets {

	static const std::string UIFileRevision="323";

	void RegisterLoaders(resource::File &File);
	inline void RegisterLoaders(resource::File *File) { RegisterLoaders(*File); }

	gge::resource::File &LoadWidgets(const std::string &filename);

	void InitializeApplication(const std::string &systemname, const std::string &windowtitle, const std::string &uifile, int width=700, int height=550, os::IconHandle icon=0);

	inline void InitializeApplication(const std::string &systemname, const std::string &windowtitle, int width, int height, const std::string &uifile, os::IconHandle icon=0) {
		InitializeApplication(systemname, windowtitle, uifile, width, height, icon);
	}

	void InitializeApplication(const std::string &systemname, const std::string &windowtitle, int width=700, int height=550, os::IconHandle icon=0);

	void Initialize(GGEMain &Main, int TopLevelOrder=1);

	extern VirtualPanel TopLevel;
	extern utils::Collection<WidgetBase> DrawQueue;
	extern gge::resource::File *WidgetFile;


	namespace AutosizeModes {
		enum Type {
			None,
			GrowOnly,
			Autosize
		};
	};

}}
