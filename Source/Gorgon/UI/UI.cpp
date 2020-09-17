#include "../UI.h"
#include "../Widgets/Generator.h"
#include "../Widgets/Registry.h"
#include "../WindowManager.h"

namespace Gorgon { 
namespace UI {
    
    Widgets::Generator *generator = nullptr;
    
    //create a default widget registry.
    void Initialize(std::string fontname, float density, int min) {
        int fh = int(std::round(sqrt(WindowManager::Monitor::Primary().GetSize().Width / (density+0.6f))));
        
        if(fh < min) 
            fh = min;
        
        generator = new Widgets::SimpleGenerator(fh, fontname, true, density);
    }
    
}

namespace Widgets {
    Registry *Registry::active = nullptr;
}
}

