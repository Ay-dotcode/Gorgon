#include "GraphicsHelper.h"
#include <Gorgon/Graphics/FreeType.h>
#include <Gorgon/Resource/File.h>
#include <Gorgon/Resource/Font.h>

#include <Gorgon/Graphics/AdvancedPrinter.h>

#include <Gorgon/ImageProcessing/Filters.h>



std::string helptext = 
    "Key list:\n"
    "esc\tClose\n"
;


int main() {
    Application app("generictest", "Test", helptext);

    Graphics::Layer l;
    app.wind.Add(l);
    
    using namespace Gorgon::Graphics;
    
    Graphics::AdvancedTextBuilder builder;
    builder.UseHeader(Gorgon::Graphics::HeaderLevel::H1)
           .Append("Hello world.\n")
           .UseDefaultFont()
           .SetHangingIndent(10, 0)
           .Append("This is ")
           .UseBoldFont()
           .SetLetterSpacing(5)
           .Append("a bold ")
           .UseDefaultFont()
           .DefaultLetterSpacing()
           .SetColor(2)
           .Append("text.")
           .ResetFormatting()
           .LineBreak()
           .SetWrapWidth(40)
           .Append("Not a new paragraph. ")
           .UseItalicFont()
           .Append("e = mc")
           .UseSuperscript()
           .Append("2")
           .ScriptOff()
    ;
    
    Graphics::AdvancedPrinter printer;
    printer.RegisterFont(0, app.sty);
    printer.RegisterFont(Graphics::NamedFonts::H1, app.stylarge);
    printer.RegisterFont(Graphics::NamedFonts::Small, app.stysmall);
    
    
    printer.AdvancedPrint(l, builder, {10, 10}, 100);
    
    while(true) {
        Gorgon::NextFrame();
    }

    return 0;
}
