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
           .WordWrap(false)
           .Append("Hello world. ")
           .UseDefaultFont()
           .Append("Not header\n")
           .WordWrap(true)
           .SetHangingIndent(10, 0)
           .Append("This is ")
           .UseBoldFont()
           .SetLetterSpacing(5, 0)
           .Append("a bold ")
           .UseDefaultFont()
           .DefaultLetterSpacing()
           .SetColor(2)
           .Append("text. ")
           .ResetFormatting()
           .Append("e = mc")
           .UseSubscript()
           .Append("Hello there")
           .ScriptOff()
           .LineBreak()
           .SetWrapWidth(175)
           .Append("Not a new paragraph.\n")
           .UseItalicFont()
           .Append("New paragraph")
    ;
    
    Graphics::AdvancedPrinter printer;
    printer.RegisterFont(0, app.sty);
    printer.RegisterFont(Graphics::NamedFont::H1, app.stylarge);
    printer.RegisterFont(Graphics::NamedFont::Small, app.stysmall);
    printer.RegisterFont(Graphics::NamedFont::Script, app.stysmall);
    
    
    printer.AdvancedPrint(l, builder, {25, 25}, 100);
    
    while(true) {
        Gorgon::NextFrame();
    }

    return 0;
}
