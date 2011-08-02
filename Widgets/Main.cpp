
#include "Main.h"
#include "Definitions.h"
#include "Basic\Line.h"
#include "Basic\Rectangle.h"
#include "Basic\ResizableObjectResource.h"
#include "Basic\BorderData.h"

namespace gge { namespace widgets {


	void RegisterLoaders(resource::File &File) {
		File.Loaders.Add(new resource::ResourceLoader(GID::Line, LoadLineResource)); 
		File.Loaders.Add(new resource::ResourceLoader(GID::Rectangle, LoadRectangleResource)); 
		File.Loaders.Add(new resource::ResourceLoader(GID::ResizableObj, LoadResizableObjectResource)); 
		File.Loaders.Add(new resource::ResourceLoader(GID::BorderData, LoadBorderDataResource)); 
	}

	void Init(GGEMain &Main) {

	}

}}
