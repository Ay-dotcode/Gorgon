#include "GraphicsHelper.h"
#include <Gorgon/Graphics/FreeType.h>
#include <Gorgon/Resource/File.h>
#include <Gorgon/Resource/Font.h>

#include <Gorgon/Graphics/AdvancedPrinter.h>

#include <Gorgon/ImageProcessing/Filters.h>
#include <Gorgon/UI/Window.h>
#include <Gorgon/Widgets/Generator.h>




std::string helptext = 
    "Key list:\n"
    "esc\tClose\n"
;


int main() {
    basic_Application<Gorgon::UI::Window> app("generictest", "Test", helptext, 25, 0xe0);

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
           .SetWrapWidth(200)
           .Append("Not a new paragraph.\n")
           .UseItalicFont()
           .Append("New paragraph")
    ;
    
    auto &reg = (Gorgon::Widgets::SimpleGenerator&)Gorgon::Widgets::Registry::Active();
    
    Graphics::AdvancedPrinter printer;
    printer.RegisterFont(0, reg.RegularFont);
    printer.RegisterFont(Graphics::NamedFont::Bold, reg.BoldFont);
    printer.RegisterFont(Graphics::NamedFont::H1, reg.TitleFont);
    printer.RegisterFont(Graphics::NamedFont::Small, reg.InfoFont);
    printer.RegisterFont(Graphics::NamedFont::Script, reg.SmallFont);
    
    
    printer.AdvancedPrint(l, builder, {25, 25}, 150);
    
    while(true) {
        Gorgon::NextFrame();
    }

    return 0;
}
