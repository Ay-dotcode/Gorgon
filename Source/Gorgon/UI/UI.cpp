#include "../UI.h"
#include "../Widgets/Generator.h"
#include "../Widgets/Registry.h"
#include "../WindowManager.h"

namespace Gorgon { 
namespace UI {
    
    Widgets::Generator *generator = nullptr;
    
    //create a default widget registry.
    void Initialize(std::string fontname, std::string boldfontname, float density, int min) {
        
        generator = new Widgets::SimpleGenerator(FontHeight(density, min), fontname, boldfontname, true, density);
    }
    
    int FontHeight(float density, int min) {
        int fh = int(std::round(sqrt(WindowManager::Monitor::Primary().GetSize().Width / (density+0.6f))));
        
        if(fh < min) 
            fh = min;
        
        return fh;
    }

    
}

namespace Widgets {
    Registry *Registry::active = nullptr;
}
}

