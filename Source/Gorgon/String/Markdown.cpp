#include "Markdown.h"
#include "../Graphics/AdvancedTextBuilder.h"

namespace Gorgon { namespace String {
   
    std::pair<std::string, std::vector<std::string>> ParseMarkdown(const std::string &text, bool useinfofont) {
        using namespace Graphics;
        
        AdvancedTextBuilder builder;
        if(useinfofont)
            builder.SetFont(NamedFont::Info);
        
        
        auto end = text.end();
        auto &result = builder.Get();
        
        bool newline = true;
        int  linecount = 0;
        bool newpar  = false;
        bool pre = false; //preformatted
        bool preinline = false; //inline version will only have a small hspace
        int  spacecount = 0;
        bool spaceadded = false;
        int outchars = 0; //number of visible glyphs in output
        int header = 0;
        int currentheader = 0;
        bool bullet = false;
        bool eatnext = false;
        int emcnt = 0;
        bool bold = false;
        bool italic = false;
        bool strike = false;
        
        const char spc = ' ';
        const char newln = '\n';
        builder.SetParagraphSpacing(0, 40);
        Glyph g = 0, prev;
        
        for(auto it = text.begin(); it!=end; ++it) {
            prev = g;
            g = internal::decode_impl(it, end);
            
            if(eatnext) {
                eatnext = false;
                continue;
            }
            
            //handle new lines
            if(internal::isnewline(g)) {
                if(header) {
                    result.push_back(newln);
                    newpar     = true;
                    newline    = true;
                    spaceadded = false;
                    builder.SetFont(useinfofont ? NamedFont::Info : NamedFont::Regular);
                    currentheader = 0;
                    header = 0;
                }
                else if(linecount > 0 || pre) {
                    result.push_back(newln);
                    newpar     = true;
                    newline    = true;
                    spaceadded = false;
                }
                else {
                    newline = true;
                }
                
                linecount++;
                spacecount = 0;
                continue;
            }
            
            if(emcnt > 0 && g != '*') {
                Glyph next = 0;
                if(it != end) {
                    auto nit = it+1;
                    if(nit != end)
                        next = internal::decode(nit, end);
                }
                
                bool changed = false;
                if(emcnt >= 2) {
                    if(bold && prev != ' ') {
                        bold = false;
                        changed = true;
                    }
                    else if(!bold && next != ' ') {
                        bold = true;
                        changed = true;
                    }
                    emcnt -= 2;
                }
                
                if(emcnt >= 1) {
                    if(italic && prev != ' ') {
                        italic = false;
                        changed = true;
                    }
                    else if(!italic && next != ' ') {
                        italic = true;
                        changed = true;
                    }
                    emcnt--;
                }
                
                if(changed) {
                    if(bold) {
                        if(italic)
                            builder.SetFont(useinfofont ? NamedFont::BoldItalicInfo : NamedFont::BoldItalic);
                        else
                            builder.SetFont(useinfofont ? NamedFont::BoldInfo : NamedFont::Bold);
                    }
                    else {
                        if(italic)
                            builder.SetFont(useinfofont ? NamedFont::ItalicInfo : NamedFont::Italic);
                        else
                            builder.SetFont(useinfofont ? NamedFont::Info : NamedFont::Regular);
                    }
                }
                
                while(emcnt) {
                    result.push_back('*');
                    emcnt--;
                }
            }
            
            if(internal::isspace(g)) {
                spacecount++;
                
                if((!spaceadded || pre) && !newline) {
                    result.push_back(spc);
                    spaceadded = true;
                }
                else if(spacecount > 4 && pre) {
                    result.push_back(spc);
                    spaceadded = true;
                }
                else if(spacecount == 4 && newline && !pre) {
                    if(!newpar)
                        result.push_back(newln);
                    
                    builder.SetFont(NamedFont::FixedWidth);
                    builder.SetIndent(0, 50);
                    builder.VerticalSpace(0, 50);
                    builder.SetColor(Color::Code);
                    pre = true;
                    preinline = false;
                }
                
                continue;
            }
            
            if(newline) {
                Glyph next = 0;
                if(it != end) {
                    auto nit = it+1;
                    if(nit != end)
                        next = internal::decode(nit, end);
                }
                
                if(!pre && (g == '*' || g == '+' || g == '-') && (next == ' ' || next == '\t')) {
                    if(!bullet) {
                        bullet = true;
                        builder.SetParagraphSpacing(0, 20);
                        
                        builder.SetIndent(0, 75);
                        builder.SetHangingIndent(0, -50);
                        builder.SetTabWidth(0, 75);
                    }
                    
                    if(!newpar)
                        result.push_back('\n');
                    builder.Append("•\t");
                    newline = false;
                    eatnext = true;
                    continue;
                }
                else {
                    if(bullet) {
                        bullet = false;
                        builder.SetParagraphSpacing(0, 40);
                        builder.SetIndent(0, 0);
                        builder.SetHangingIndent(0, 0);
                        builder.UseDefaultTabWidth();
                        
                        if(!newpar)
                            result.push_back('\n');
                    }
                }
                
                if(!pre && g == '#') {
                    header++;
                    continue;
                }
                else if(pre && spacecount < 4) {
                    builder.SetFont(useinfofont ? NamedFont::Info : NamedFont::Regular);
                    builder.SetIndent(0, 0);
                    builder.VerticalSpace(0, 50);
                    builder.UseDefaultColor();
                    
                    pre = false;
                }
                else if(!newpar && linecount > 0 && outchars != 0 && spaceadded == 0 && !pre) {
                    result.push_back(spc);
                    spaceadded = true;
                }
                
                newline = false;
                newpar  = false;
            }
            
            if(header && header != currentheader) {
                if(!spacecount)
                    header = 0;
                
                if(header > 4)
                    header = 4;
                if(header && !newpar && outchars)
                    result.push_back('\n');
                if(header == 0)
                    builder.SetFont(useinfofont ? NamedFont::Info : NamedFont::Regular);
                else
                    builder.UseHeader((Graphics::HeaderLevel)((int)Graphics::HeaderLevel::H1 + header - 1));
                currentheader = header;
            }
            
            spacecount = 0;
            linecount = 0;
            newpar = false;
            newline = false;
            
            if(g == '`') {
                if(!pre) {
                    pre = true;
                    preinline = true;
                    builder.SetFont(NamedFont::FixedWidth);
                    builder.HorizontalSpace(0, 20);
                    builder.SetColor(Color::Code);
                    continue;
                }
                else if(preinline) {
                    pre = false;
                    builder.SetFont(useinfofont ? NamedFont::Info : NamedFont::Regular);
                    builder.HorizontalSpace(0, 20);
                    builder.UseDefaultColor();
                    continue;
                }
            }
            
            if(!pre) {
                if(g == '*' && emcnt < 3) {
                    emcnt++;
                    g = prev;
                    continue;
                }
            }
            
            String::AppendUnicode(result, g);
            outchars++;
            spaceadded = false;
        }
        
        return {result, {}};
    }
    
} }
