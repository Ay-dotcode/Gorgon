#include "../UI.h"
#include "../Widgets/Generator.h"
#include "../Widgets/Registry.h"
#include "../WindowManager.h"

namespace Gorgon { 
namespace UI {
    
    Widgets::Generator *generator = nullptr;
    
    //create a default widget registry.
    void Initialize(std::string fontname, int density, int min) {
        int fh = WindowManager::Monitor::Primary().GetSize().Width / density;
        
        if(fh < min) 
            fh = min;
        
        generator = new Widgets::SimpleGenerator(fh, fontname);
    }
    
}

namespace Widgets {
    Registry *Registry::active = nullptr;
}
}

