#include "GraphicsHelper.h"
#include <Gorgon/Graphics/FreeType.h>
#include <Gorgon/Resource/File.h>
#include <Gorgon/Resource/Font.h>

#include <Gorgon/Graphics/AdvancedPrinter.h>

#include <Gorgon/ImageProcessing/Filters.h>
#include <Gorgon/UI/Window.h>
#include <Gorgon/Widgets/Generator.h>
#include <Gorgon/CGI/Marking.h>


std::string helptext = 
    "Key list:\n"
    "esc\tClose\n"
;


int main() {
    basic_Application<Gorgon::UI::Window> app("generictest", "Test", helptext, 25, 0x10);

    Graphics::Layer l;
    app.wind.Add(l);
    
    using namespace Gorgon::Graphics;
    
    Graphics::AdvancedTextBuilder builder;
    builder.UseHeader(Gorgon::Graphics::HeaderLevel::H1)
           .WordWrap(false)
           .StartRegion(4)
           .SetSelectedTextColor(Graphics::Color::Black)
           .SetSelectionPadding(12)
           .SetSelectionImage(0)
           .SetSelectedBackgroundColor(Graphics::Color::BrightYellow)//overrides
           .SetSelectionPadding(0)
           .Append("Hello world. ")
           .EndRegion(4)
           .UseDefaultFont()
           .StartRegion(5)
           .Justify(true)
           .UnderlineSettings(false, false)
           .Append("Not header")
           .EndRegion(5)
           .Append("\n")
           .SetUnderlineColor(2)
           .WordWrap(true)
           .SetHangingIndent(10, 0)
           .StartSelection()
           .Append("This ")
           .EndSelection()
           .StartRegion(0)
           .Append("is")
           .UseBoldFont()
           .SetLetterSpacing(3, 0)
           .HorizontalSpace(-5)
           .VerticalSpace(15)
           .Append(" a bold ")
           .EndRegion(0)
           .UseDefaultFont()
           .DefaultLetterSpacing()
           .SetColor(2, 171)
           .Append("t.ext. ")
           .ResetFormatting()
           .Append("e = mc")
           .UseSubscript()
           .SetColor(Gorgon::Graphics::Color::Aqua)
           .StartRegion(6)
           .Append("Hello there")
           .EndRegion(6)
           .UseDefaultColor()
           .ScriptOff()
           .StartRegion(1)
           .Append("And")
           .EndRegion(1)
           .UseSuperscript()
           .StartRegion(3)
           .Append("back")
           .EndRegion(3)
           .SetParagraphSpacing(0, 50)
           .ScriptOff()
           .LineBreak()
           .SetWrapWidth(200)
           .SetTabWidth(0, 0, 2500)
           .StartRegion(2)
           .Append("Not a\tnew\tparagraph. Just a line break\n")
           .EndRegion(2)
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
    
    auto icon = Triangle(5, 5);
    icon.Prepare();
    printer.RegisterImage(0, icon); //leak!
    
    printer.GetBreakingLetters().push_back('.');
    
    printer.RegisterColor(2, Gorgon::Graphics::Color::BrightRed, Gorgon::Graphics::Color::DarkOlive);
    
    //l.Draw(reg.Background.Regular);

    Graphics::Bitmap markings(500, 500, Gorgon::Graphics::ColorMode::RGBA);
    markings.Clear();
    markings.Prepare();
    markings.Draw(l, 0, 0);
    
    auto regions = printer.AdvancedPrint(l, builder, {25, 25}, 150);
    
    std::vector<Graphics::RGBA> regioncolor = {
        Graphics::Color::Red,
        Graphics::Color::LightBrown,
        Graphics::Color::Green,
        Graphics::Color::Grey,
        Graphics::Color::LightBlue,
        Graphics::Color::Magenta,
        Graphics::Color::BrightPurple,
    };
    /*
    for(auto r : regions) {
        Gorgon::CGI::DrawBounds(
            markings, 
            r.Bounds,
            1, Gorgon::CGI::SolidFill<>(regioncolor[r.ID])
        );
    }
    */
    markings.Prepare();

    while(true) {
        Gorgon::NextFrame();
    }

    return 0;
}
