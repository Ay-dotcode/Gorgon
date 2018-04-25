#include "GraphicsHelper.h"
#include <Gorgon/Graphics/FreeType.h>


std::string helptext = 
    "Key list:\n"
	"esc\tClose\n"
;


int main() {
	Application app("generictest", "Test", helptext);

	Graphics::Layer l;
    app.wind.Add(l);
    
    using namespace Gorgon::Graphics;
    
    FreeType f("/usr/share/fonts/liberation/LiberationSans-Regular.ttf");
    f.LoadMetrics(12);
    
    std::cout<<f.GetFamilyName()<<": "<<f.GetStyleName()<<std::endl;
    std::cout<<"Preset sizes: "<<f.GetPresetSizes().size()<<std::endl;
    std::cout<<"Is scalable: "<<f.IsScalable()<<std::endl;
    std::cout<<"Height: "<<f.GetHeight()<<std::endl;
    std::cout<<"Max width: "<<f.GetMaxWidth()<<std::endl;
    std::cout<<"Baseline: "<<f.GetBaseLine()<<std::endl;
    std::cout<<"Underline: "<<f.GetUnderlineOffset()<<std::endl;
    std::cout<<"Line thickness: "<<f.GetLineThickness()<<std::endl;
    
    f.LoadGlyphs(32, 127);
    
    BasicFont r(f);
    
    r.Print(l, "This is a test text\nwith second line jj\nWith kerning: AV T. Ta", 100, 0, 300, TextAlignment::Right);

	while(true) {
		Gorgon::NextFrame();
	}

	return 0;
}
