#include "Inputbox.h"

#include "../Graphics/Font.h"
#include "../WindowManager.h"

namespace Gorgon { namespace Widgets { namespace internal {
    
    bool Inputbox_base::KeyEvent(Input::Key key, float state) {
        namespace Keycodes = Input::Keyboard::Keycodes;

        if(repeater.KeyEvent(key, state))
            return true;

        if(state) {
            if(Input::Keyboard::CurrentModifier == Input::Keyboard::Modifier::None) {
                switch(key) {
                case Keycodes::Home:
                    selstart = {0, 0};
                    sellen = {0, 0};
                    updateselection();

                    return true;

                case Keycodes::End:
                    selstart = {Length(), (int)display.size()};
                    sellen = {0, 0};
                    updateselection();

                    return true;

                case Keycodes::Backspace:
                    if(sellen.byte == 0) {
                        if(selstart.glyph != 0) {
                            moveselleft();

                            display.erase(selstart.byte, String::UTF8Bytes(display[selstart.byte]));
                            glyphcount--;

                            updatevalue();
                            updatevaluedisplay(false);
                            updateselection();
                        }

                    }
                    else {
                        eraseselected();

                        updatevalue();
                        updatevaluedisplay(false);
                        updateselection();
                    }

                    return true;
                case Keycodes::Delete:
                    if(sellen.byte == 0) {
                        if(selstart.glyph < glyphcount) {
                            display.erase(selstart.byte, String::UTF8Bytes(display[selstart.byte]));
                            glyphcount--;

                            updatevalue();
                            updatevaluedisplay(false);
                        }
                    }
                    else {
                        eraseselected();

                        updatevalue();
                        updatevaluedisplay(false);
                        updateselection();
                    }

                    return true;
                }
            }
            else if(Input::Keyboard::CurrentModifier == Input::Keyboard::Modifier::Ctrl) {
                switch(key) {
                case Keycodes::A:
                    SelectAll();

                    return true;

                case Keycodes::C: {
                    std::string s = GetSelectedText();
                    if(!s.empty()) {
                        WindowManager::SetClipboardText(s);
                    }
                    
                    return true;
                }
                
                case Keycodes::V:
                    auto s = WindowManager::GetClipboardText();
                    
                    if(s.empty())
                        return true;
                    
                    eraseselected();
                    
                    int gcnt = String::UnicodeGlyphCount(s);
                    
                    display.insert(selstart.byte, s);
                    
                    selstart.byte  += s.size();
                    selstart.glyph += gcnt;
                    
                    glyphcount += gcnt;

                    updatevalue();
                    updatevaluedisplay(false);
                    updateselection();
                    
                    return true;
                }
            }
        }

        return false;
    }

    bool Inputbox_base::CharacterEvent(Char c) {
        if(sellen.byte != 0) {
            eraseselected();
        }

        if(selstart.byte == display.size())
            String::AppendUnicode(display, c);
        else
            String::InsertUnicode(display, selstart.byte, c);

        glyphcount++;

        selstart.glyph++;
        selstart.byte += String::UnicodeUTF8Bytes(c);

        updatevalue();
        updatevaluedisplay(false);
        updateselection();

        return true;
    }


    void Inputbox_base::updateselection() {
        //for font
        int idx = stack.IndexOfTag(UI::ComponentTemplate::ContentsTag);

        //no contents??!!
        if(idx == -1)
            return;

        //contents is not a textholder
        auto &temp = stack.GetTemplate(idx);
        if(temp.GetType() != UI::ComponentType::Textholder)
            return;

        auto &textt = dynamic_cast<const UI::TextholderTemplate&>(temp);

        //no renderer is set or renderer is not in valid state
        if(!textt.IsReady())
            return;

        auto &renderer = textt.GetRenderer();
        
        if(selstart.byte < 0) {
            selstart  = {0, 0};
        }
        if(selstart.byte + sellen.byte < 0) {
            sellen = selstart;
        }
        
        if(selstart.byte > display.size()) { //equal is fine
            selstart = {glyphcount, (int)display.size()};
        }        
        if(selstart.byte + sellen.byte >= display.size()) {
            sellen = {glyphcount - selstart.glyph, (int)display.size() - selstart.byte};
        }

        auto pos = selstart.glyph;

        if(sellen.glyph == allselected)
            pos = (int)display.length(); //this is ok, bytelen >= glyphlen
        else
            pos += sellen.glyph;

        std::cout<<selstart.glyph<<"\t"<<selstart.byte<<"\t|\t"<<sellen.glyph<<"\t"<<sellen.byte<<"\t|\t"<<glyphcount<<std::endl;

        Geometry::Point location;
        
        if(display == "") {
            location = {0, 0};
        }
        else {
            location = renderer.GetPosition(display, stack.TagBounds(UI::ComponentTemplate::ContentsTag).Width(), pos).TopLeft();
        }
        stack.SetTagLocation(UI::ComponentTemplate::CaretTag, location);
    }

} } }
