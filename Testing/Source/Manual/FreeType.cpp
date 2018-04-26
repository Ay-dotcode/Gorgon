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
    
    /*std::vector<Byte> data;
    std::ifstream ttf("/usr/share/fonts/gnu-free/FreeSans.ttf");
    
    char b;
    while(ttf.read(&b, 1))
        data.push_back(b);*/
    
    FreeType f;
    f.LoadFile("/usr/share/fonts/urw-base35/NimbusRoman-Regular.otf");
    f.LoadMetrics(16);
    
    std::cout<<f.GetFamilyName()<<": "<<f.GetStyleName()<<std::endl;
    std::cout<<"Preset sizes: "<<f.GetPresetSizes().size()<<std::endl;
    std::cout<<"Is scalable: "<<f.IsScalable()<<std::endl;
    std::cout<<"Height: "<<f.GetHeight()<<std::endl;
    std::cout<<"Max width: "<<f.GetMaxWidth()<<std::endl;
    std::cout<<"Baseline: "<<f.GetBaseLine()<<std::endl;
    std::cout<<"Underline: "<<f.GetUnderlineOffset()<<std::endl;
    std::cout<<"Line thickness: "<<f.GetLineThickness()<<std::endl;
    
    //f.LoadGlyphs(32, 127);
    
    BasicFont r(f);
    
    r.Print(l, "This is a test text\nwith second line jj\nWith kerning: AV T. Ta.\nTürkçe ve Unicode desteği!!", 100, 10, 300, TextAlignment::Right);

	while(true) {
		Gorgon::NextFrame();
	}

	return 0;
}
