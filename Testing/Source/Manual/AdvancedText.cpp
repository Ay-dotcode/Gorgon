#include "GraphicsHelper.h"
#include <Gorgon/Graphics/FreeType.h>
#include <Gorgon/Resource/File.h>
#include <Gorgon/Resource/Font.h>

#include <Gorgon/Graphics/AdvancedRenderer.h>

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
    builder.UseHeader(Gorgon::Graphics::AdvancedTextBuilder::H1)
           .Append("Hello world.\n")
           .UseDefaultFont()
           .SetHangingIndent(10, 0)
           .Append("This is ")
           .UseBoldFont()
           .Append("a bold ")
           .UseDefaultFont()
           .SetColor(2)
           .Append("text.")
           .ResetFormatting()
           .LineBreak()
           .Append("Not a paragraph")
    ;
    
    app.sty.Print(l, builder, 10, 10, 100);
    
    while(true) {
        Gorgon::NextFrame();
    }

    return 0;
}
