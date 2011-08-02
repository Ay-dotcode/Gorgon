
#include "Main.h"
#include "Definitions.h"
#include "Basic\Line.h"
#include "Basic\Rectangle.h"
#include "Basic\ResizableObjectResource.h"
#include "Basic\BorderData.h"
#include "Basic\Placeholder.h"

namespace gge { namespace widgets {


	void RegisterLoaders(resource::File &File) {
		File.AddGameLoaders();
		File.Loaders.Add(new resource::ResourceLoader(GID::Line, LoadLineResource)); 
		File.Loaders.Add(new resource::ResourceLoader(GID::Rectangle, LoadRectangleResource)); 
		File.Loaders.Add(new resource::ResourceLoader(GID::ResizableObj, LoadResizableObjectResource)); 
		File.Loaders.Add(new resource::ResourceLoader(GID::BorderData, LoadBorderDataResource)); 
		File.Loaders.Add(new resource::ResourceLoader(GID::Placeholder, LoadPlaceholderResource)); 
	}

	void Init(GGEMain &Main) {

	}

}}
