//Copy Resources/Testing/Victoria to Testing/Runtime (in VS you need to fix running directory)

#include <Gorgon/Window.h>
#include <Gorgon/Main.h>
#include "Gorgon/Graphics/Bitmap.h"
#include "Gorgon/Graphics/Layer.h"
#include <Gorgon/Resource/File.h>
#include <Gorgon/Resource/Font.h>
#include "Gorgon/Graphics/BitmapFont.h"


using Gorgon::Window;
using Gorgon::Geometry::Point;
namespace Graphics = Gorgon::Graphics;
namespace Input = Gorgon::Input;
namespace Resource = Gorgon::Resource;


int main() {
	
	Gorgon::Initialize("Font-test");

    system("pwd");
    system("cd");
    
        
	Window wind({800, 600}, "windowtest", "Font Test");
	Graphics::Initialize();

	wind.DestroyedEvent.Register([]{
		exit(0);
	});

	Graphics::Layer l;
	wind.Add(l);

    //Background
	Graphics::Bitmap img3({50, 50}, Graphics::ColorMode::Grayscale);

	for(int x = 0; x<img3.GetWidth(); x++)
		for(int y = 0; y<img3.GetHeight(); y++) {
			if((x/(img3.GetWidth()/2)) != (y/(img3.GetHeight()/2)))
				img3({x, y}, 0) = 0x10;
			else
				img3({x, y}, 0) = 0x30;
		}

	img3.Prepare();
	img3.DrawIn(l);

    //destruction tests
    {
        Graphics::BitmapFont fnt;
        fnt.ImportFolder("Victoria", Graphics::BitmapFont::Automatic, 0, "", false);
    }
    {
        Graphics::BitmapFont fnt;
        fnt.ImportFolder("Victoria", Graphics::BitmapFont::Automatic, 0, "");
    }

    //save to load later
    {
        Graphics::BitmapFont::ImportOptions options;
        options.pack = false;
        Graphics::BitmapFont fnt;
        fnt.ImportFolder("Victoria", Graphics::BitmapFont::Automatic, 0, "", options);
        
        std::cout<<"digit w: "<<fnt.GetDigitWidth()<<std::endl;
        std::cout<<"x-space: "<<fnt.GetGlyphSpacing()<<std::endl;
        std::cout<<"height: "<<fnt.GetHeight()<<std::endl;
        std::cout<<"line t: "<<fnt.GetLineThickness()<<std::endl;
        std::cout<<"max w: "<<fnt.GetMaxWidth()<<std::endl;
        std::cout<<"underline off: "<<fnt.GetUnderlineOffset()<<std::endl;
        std::cout<<"baseline: "<<fnt.GetBaseLine()<<std::endl;
        std::cout<<"is fixed: "<<fnt.IsFixedWidth()<<std::endl;
        std::cout<<"is ASCII: "<<fnt.IsASCII()<<std::endl;
        
        Resource::File f;
        Resource::Font fr(fnt);
        f.Root().Add(fr);
        f.Save("font-test.gor");
		f.Root().Remove(fr);
    }
    
    //destruction test
    {
        Resource::File f2;
        f2.LoadFile("font-test.gor");
        f2.Prepare();
        f2.Discard();
        
        Graphics::BitmapFont fnt = std::move(dynamic_cast<Graphics::BitmapFont&>(f2.Root().Get<Resource::Font>(0).GetRenderer()));
    }
    
    //load to use
    Resource::File f2;
    f2.LoadFile("font-test.gor");
    f2.Prepare();
    f2.Discard();
    
    Graphics::BitmapFont fnt = std::move(dynamic_cast<Graphics::BitmapFont&>(f2.Root().Get<Resource::Font>(0).GetRenderer()));
    
    
    std::cout<<"\n===============\n"<<"After loading:"<<std::endl;
    std::cout<<"digit w: "<<fnt.GetDigitWidth()<<std::endl;
    std::cout<<"x-space: "<<fnt.GetGlyphSpacing()<<std::endl;
    std::cout<<"height: "<<fnt.GetHeight()<<std::endl;
    std::cout<<"line t: "<<fnt.GetLineThickness()<<std::endl;
    std::cout<<"max w: "<<fnt.GetMaxWidth()<<std::endl;
    std::cout<<"underline off: "<<fnt.GetUnderlineOffset()<<std::endl;
    std::cout<<"baseline: "<<fnt.GetBaseLine()<<std::endl;
    std::cout<<"is fixed: "<<fnt.IsFixedWidth()<<std::endl;
    std::cout<<"is ASCII: "<<fnt.IsASCII()<<std::endl;
    
	Graphics::StyledRenderer sty(fnt);
	sty.UseFlatShadow({0.f, 1.0f}, {1.f, 1.f});
	sty.SetColor({0.6f, 1.f, 1.f});
	sty.JustifyLeft();
    
	sty.Print(l, "\xf0\x90\x8d\x88Lor|em ipsum\xe2\x80\xa8""folor sit amet, consecteturadipiscingelitseddoeiusmoftemporincididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.", 0, 0, 300);
    
    sty.Underline();
	sty.Print(l, "Name\tSurname\nMarvin\tthe Robot\nMe\tMyself", 250, 200);
    
    sty.SetUnderline(false);
	sty.DisableShadow();
    sty.SetColor({1.f, 0.4f, 0.4f});
	sty.Print(l, "First text should be justified.\nPrevious text should have tabs and underlined.\nThis text should not have shadow and should be red.", 250, 240);
    
    Graphics::BitmapFont fixedsize_original;
    Graphics::BitmapFont::ImportOptions options;
    options.converttoalpha = Gorgon::YesNoAuto::Auto;
    std::cout<<"Imported "<<fixedsize_original.ImportAtlas("fixed-font.bmp", {7, 9}, 0x20, false, options)<<" glyphs."<<std::endl;
    fixedsize_original.Print(l, "Hello!, fixed sized import is working.\nKerning example: Ta, T.", 350, 2);
    
    Graphics::BitmapFont fixedsize_repack;
    std::cout<<"Imported "<<fixedsize_repack.ImportAtlas("fixed-font.bmp", {7, 9}, 0x20, true, options)<<" glyphs."<<std::endl;
    fixedsize_repack.Print(l, "Hello!, fixed sized import is working.\nKerning example: Ta, T.", 350, 6+fixedsize_original.GetLineGap() * 2);
    

	while(true) {
		Gorgon::NextFrame();
	}
}
