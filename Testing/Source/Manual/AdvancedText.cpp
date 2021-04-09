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
    basic_Application<Gorgon::UI::Window> app("generictest", "Test", helptext, 25, 0x20);

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
           .HorizontalSpace(-5)
           .VerticalSpace(15)
           .Append("a bold ")
           .UseDefaultFont()
           .DefaultLetterSpacing()
           .SetColor(2, 171)
           .Append("t.ext. ")
           .ResetFormatting()
           .Append("e = mc")
           .UseSubscript()
           .SetColor(Gorgon::Graphics::Color::DarkAqua)
           .Append("Hello there")
           .UseDefaultColor()
           .ScriptOff()
           .Append("And")
           .UseSubscript()
           .Append("back")
           .SetParagraphSpacing(0, 0)
           .ScriptOff()
           .LineBreak()
           .SetWrapWidth(200)
           .SetTabWidth(0, 0, 2500)
           .Append("Not a\tnew\tparagraph. Just a line break\n")
           .UseItalicFont()
           .Append("New\tparagraph ")
           .Append("D")
           .SetLetterOffset({0,2})
           .Append("a")
           .SetLetterOffset({0,4})
           .Append("n")
           .SetLetterOffset({0,5})
           .Append("c")
           .SetLetterOffset({0,6})
           .Append("i")
           .SetLetterOffset({0,5})
           .Append("n")
           .SetLetterOffset({0,4})
           .Append("g")
           .SetLetterOffset({0,2})
           .Append(".")
           .SetLetterOffset({0,0})
           .Append(".")
    ;
    
    auto &reg = (Gorgon::Widgets::SimpleGenerator&)Gorgon::Widgets::Registry::Active();
    
    Graphics::AdvancedPrinter printer;
    printer.RegisterFont(0, reg.RegularFont);
    printer.RegisterFont(Graphics::NamedFont::Bold, reg.BoldFont);
    printer.RegisterFont(Graphics::NamedFont::H1, reg.TitleFont);
    printer.RegisterFont(Graphics::NamedFont::Small, reg.InfoFont);
    printer.RegisterFont(Graphics::NamedFont::Script, reg.SmallFont);
    
    printer.GetBreakingLetters().push_back('.');
    
    printer.RegisterColor(2, Gorgon::Graphics::Color::Red, Gorgon::Graphics::Color::LightYellow);
    
    l.Draw(reg.Background.Regular);
    printer.AdvancedPrint(l, builder, {25, 25}, 150);
    
    while(true) {
        Gorgon::NextFrame();
    }

    return 0;
}
