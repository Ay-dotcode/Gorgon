#include "GraphicsHelper.h"
#include <Gorgon/Graphics/FreeType.h>
#include <Gorgon/Resource/File.h>
#include <Gorgon/Resource/Font.h>


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
#ifdef WIN32
    f.LoadFile("C:/Windows/Fonts/arial.ttf");
#else
	f.LoadFile("/usr/share/fonts/liberation/LiberationSerif-Regular.ttf");
#endif
    f.LoadMetrics(12);
    
    std::cout<<f.GetFamilyName()<<": "<<f.GetStyleName()<<std::endl;
    std::cout<<"Preset sizes: "<<f.GetPresetSizes().size()<<std::endl;
    std::cout<<"Is scalable: "<<f.IsScalable()<<std::endl;
    std::cout<<"Height: "<<f.GetHeight()<<std::endl;
    std::cout<<"Max width: "<<f.GetMaxWidth()<<std::endl;
    std::cout<<"Baseline: "<<f.GetBaseLine()<<std::endl;
	std::cout<<"Underline: "<<f.GetUnderlineOffset()<<std::endl;
	std::cout<<"Line thickness: "<<f.GetLineThickness()<<std::endl;
	std::cout<<"Line gap: "<<f.GetLineGap()<<std::endl;
    std::cout<<"Kerning: "<<f.HasKerning()<<std::endl;
    
    f.LoadGlyphs({0, {32, 127}});
    
    auto f2 = f.CopyToBitmap();
    

    Resource::File file;
    Resource::Font fr(f2);
    file.Root().Add(fr);
    file.Save("freetype-test.gor");
    file.Root().Remove(fr);
    
    file.Destroy();
    file.LoadFile("freetype-test.gor");
    auto &ff = file.Root().Get<Resource::Font>(0).GetRenderer();
    file.Prepare();
    
    BasicFont f3(ff);
    
    f3.Print(l, "This is a test text\nwith second line jj\nWith kerning: AV T. Ta.\nTürkçe ve Unicode desteği!!", 300, 100, 300, TextAlignment::Right);

    f.Print(l, "This is a test text\nwith second line jj\nWith kerning: AV T. Ta.\nTürkçe ve Unicode desteği!!", 0, 100, 300, TextAlignment::Right);

	std::cout<<f2.GetLineGap()<<std::endl;

	while(true) {
		Gorgon::NextFrame();
	}

	return 0;
}
