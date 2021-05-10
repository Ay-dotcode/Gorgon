#include "../UI.h"
#include "../Widgets/Generator.h"
#include "../Widgets/Registry.h"
#include "../WindowManager.h"

namespace Gorgon { 
namespace UI {
    
    //create a default widget registry.
    void Initialize(float density, int min) {
        auto gen = new Widgets::SimpleGenerator();
        gen->Init(FontHeight(density, min), "", "", density);
        gen->Activate();
    }
    
    void Initialize(Widgets::Registry &reg) {
        reg.Activate();
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

