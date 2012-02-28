#pragma once

//This file is the main entry file of Widgets, 
// this contains the function that allows loading
// widget files
// 

#include "..\Resource\File.h"
#include "..\Engine\GGEMain.h"
#include "Base\VirtualPanel.h"


namespace gge { namespace widgets {

	void RegisterLoaders(resource::File &File);
	inline void RegisterLoaders(resource::File *File) { RegisterLoaders(*File); }

	void Init(GGEMain &Main);

	extern VirtualPanel TopLevel;
	extern utils::Collection<WidgetBase> DrawQueue;


	namespace AutosizeModes {
		enum Type {
			None,
			GrowOnly,
			Autosize
		};
	};

}}
