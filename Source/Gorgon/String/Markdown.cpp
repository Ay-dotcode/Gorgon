#include "Markdown.h"
#include "../Graphics/AdvancedTextBuilder.h"

namespace Gorgon { namespace String {
   
    std::string readuntilornewline(Graphics::Glyph c, std::string::const_iterator &it, const std::string::const_iterator &end) {
        if(it == end)
            return "";
        
        ++it;
        
        std::string acc;
        
        Graphics::Glyph g = 0, prev;
        
        for(; it != end; it++) {
            prev = g;
            g = Graphics::internal::decode_impl(it, end);
            
            if((g == c && prev != '\\') || g == '\n')
                return acc;
            
            String::AppendUnicode(acc, g);
        }
        
        return acc;
    }
    
    std::pair<std::string, std::vector<MarkDownLink>> ParseMarkdown(const std::string &text, bool useinfofont) {
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
        int tildecnt = 0;
        int regioncount = 0;
        std::vector<MarkDownLink> links;
        
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
                else if(spacecount > 1) {
                    builder.LineBreak();
                    spaceadded = true;
                }
                else if(prev == '\\') {
                    result.pop_back();
                    builder.LineBreak();
                    spaceadded = true;
                }
                else {
                    newline = true;
                }
                
                linecount++;
                spacecount = 0;
                continue;
            }
            
            if(emcnt > 0 && g != '*') {
                bool changed = false;
                if(emcnt >= 2) {
                    if(bold && prev != ' ') {
                        bold = false;
                        changed = true;
                        emcnt -= 2;
                    }
                    else if(!bold && g != ' ') {
                        bold = true;
                        changed = true;
                        emcnt -= 2;
                    }
                }
                
                if(emcnt >= 1) {
                    if(italic && prev != ' ') {
                        italic = false;
                        changed = true;
                        emcnt -= 1;
                    }
                    else if(!italic && g != ' ') {
                        italic = true;
                        changed = true;
                        emcnt -= 1;
                    }
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
                    spaceadded = false;
                    emcnt--;
                }
            }
            if(tildecnt > 0 && g != '~') {
                if(tildecnt >= 2) {
                    if(strike && prev != ' ') {
                        strike = false;
                        builder.Strikethrough(false);
                        tildecnt -= 2;
                    }
                    else if(!strike && g != ' ') {
                        strike = true;
                        builder.Strikethrough(true);
                        tildecnt -= 2;
                    }
                }
                while(tildecnt) {
                    result.push_back('~');
                    spaceadded = false;
                    tildecnt--;
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
                else if(spacecount == 4 && newpar && !pre) {
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
                        next = internal::decode_impl(nit, end);
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
                        
                        newpar = true;
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
            
            if(!pre && g == '[') {
                auto p1 = readuntilornewline(']', it, end);
                
                auto final = 0;
                if(it != end)
                    final = *it;
                
                Glyph next = 0;
                auto nit = it;
                if(nit != end) {
                    nit++;
                    if(nit != end)
                        next = internal::decode_impl(nit, end);
                }
                
                if(final == ']' && next == ':' && newpar) { //link definition
                    result.pop_back(); //pop \n
                    
                    it = nit;
                    while(it != end && it<end-1 && *(it+1) == ' ') //eat spaces if any
                        it++;
                    
                    auto p2 = readuntilornewline(' ', it, end);
                    
                    while(it != end && it<end-1 && *(it+1) == ' ') //eat spaces if any
                        it++;
                    
                    std::string p3;
                    if(it != end && *it == ' ') { //has title
                        p3 = readuntilornewline('\n', it, end);
                        if(p3 != "" && (p3[0] == '"' || p3[0] == '\''))
                            p3 = p3.substr(1);
                        if(p3 != "" && (p3.back() == '"' || p3.back() == '\''))
                            p3.pop_back();
                    }
                    
                    auto lit = std::find_if(links.begin(), links.end(), [p1](auto v) {
                        return v.ID == p1;
                    });
                    
                    if(lit == links.end()) {
                        links.push_back({-1, "", p1, p2, p3});
                    }
                    else {
                        lit->Destination = p2;
                        lit->Title       = p3;
                    }
                    
                    continue;
                }
                else if(final == ']' && next == '[') { //link id
                    it = nit;
                    while(it != end && it<end-1 && *(it+1) == ' ') //eat spaces if any
                        it++;
                    
                    auto p2 = readuntilornewline(']', it, end);
                    
                    if(it == end || *it != ']') { //not well-formed
                        result.push_back('[');
                        result += p1;
                        result.push_back(']');
                        it = nit; //roll back
                        
                        spaceadded = false;
                        continue;
                    }
                    
                    auto lit = std::find_if(links.begin(), links.end(), [p1](auto v) {
                        return v.ID == p1;
                    });
                    
                    if(lit == links.end()) {
                        links.push_back({regioncount, p1, p2, "", ""});
                    }
                    else {
                        lit->Index = regioncount;
                        lit->Text = p1;
                    }
                    
                    //render
                    builder.StartRegion(regioncount);
                    builder.Underline();
                    builder.Append(p1);
                    builder.Underline(false);
                    builder.EndRegion(regioncount);
                    
                    regioncount++;
                    spaceadded = false;
                    continue; //we are done
                }
                else if(final == ']' && next == '(') { //inline link
                    it = nit;
                    while(it != end && it<end-1 && *(it+1) == ' ') //eat spaces if any
                        it++;
                    
                    //read until space or ]
                    
                    links.push_back({regioncount, p1, "", "", ""});
                    
                    //render
                    builder.StartRegion(regioncount);
                    builder.Underline();
                    builder.Append(p1);
                    builder.Underline(false);
                    builder.EndRegion(regioncount);
                    
                    regioncount++;
                    spaceadded = false;
                    continue; //we are done
                }
                else { //not a link
                    result.push_back('[');
                    result += p1;
                    if(it != end && *it == ']') //either ] or \n
                        result.push_back(']');
                    else
                        it--; //let \n to be processed later
                    spaceadded = false;
                }
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
                if(g == '~' && tildecnt < 2) {
                    tildecnt++;
                    g = prev;
                    continue;
                }
            }
            
            String::AppendUnicode(result, g);
            outchars++;
            spaceadded = false;
        }
        
        return {result, links};
    }
    
} }
