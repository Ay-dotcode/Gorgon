#include "Inputbox.h"

#include "../Graphics/Font.h"
#include "../WindowManager.h"

namespace Gorgon { namespace Widgets { namespace internal {
    
    Inputbox_base::Inputbox_base(const UI::Template& temp) : UI::ComponentStackWidget(temp) {
            
        stack.HandleMouse(Input::Mouse::Button::Left);
            
        stack.SetMouseUpEvent([this](auto tag, auto location, auto button) { mouseup(tag, location, button); });
        stack.SetMouseMoveEvent([this](auto tag, auto location) { mousemove(tag, location); });
        stack.SetMouseDownEvent([this](auto tag, auto location, auto button) { 
            if(IsFocused())
                mousedown(tag, location, button); 

            if(allowfocus() && button == Input::Mouse::Button::Left)
                Focus();
        });
        
        repeater.Register(Input::Keyboard::Keycodes::Left);
        repeater.Register(Input::Keyboard::Keycodes::Right);

        repeater.SetRepeatOnPress(true);

        repeater.Repeat.Register([this](Input::Key key) {
            if(Input::Keyboard::CurrentModifier == Input::Keyboard::Modifier::Shift) {
                if(sellen.byte == allselected) {
                    sellen.byte  = (int)display.size() - selstart.byte;
                    sellen.glyph = glyphcount - selstart.glyph;
                }

                if(key == Input::Keyboard::Keycodes::Left) {
                    if(sellen.glyph + selstart.glyph > 0) {
                        sellen.glyph--;
                        sellen.byte--;

                        //if previous byte is unicode continuation point, go further before
                        while((sellen.byte + selstart.byte) && (display[sellen.byte + selstart.byte] & 0b11000000) == 0b10000000)
                            sellen.byte--;
                    }

                    updateselection();
                }
                else if(key == Input::Keyboard::Keycodes::Right) {
                    if(sellen.glyph + selstart.glyph < glyphcount) {
                        sellen.glyph++;
                        sellen.byte += String::UTF8Bytes(display[selstart.byte + sellen.byte]);
                    }

                    updateselection();
                }
            }
            else if(Input::Keyboard::CurrentModifier == Input::Keyboard::Modifier::None) {
                if(sellen.byte == allselected) {
                    sellen = {0, 0};

                    if(key == Input::Keyboard::Keycodes::Left) {
                        selstart = {0, 0};
                    }
                    else if(key == Input::Keyboard::Keycodes::Right) {
                        selstart = {Length(), (int)display.size()};
                    }

                    updateselection();
                }
                else if(sellen.byte != 0) {
                    if(key == Input::Keyboard::Keycodes::Left) {
                        if(sellen.byte < 0) {
                            selstart += sellen;

                            moveselleft();
                        }
                    }
                    else if(key == Input::Keyboard::Keycodes::Right) {
                        if(sellen.byte > 0) {
                            selstart += sellen;

                            moveselright();
                        }
                    }

                    sellen = {0, 0};

                    updateselection();
                }
                else {
                    if(key == Input::Keyboard::Keycodes::Left) {
                        moveselleft();
                        updateselection();
                    }
                    else if(key == Input::Keyboard::Keycodes::Right) {
                        moveselright();
                        updateselection();
                    }
                }
            }
        });
    }

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
            pos = glyphcount;
        else
            pos += sellen.glyph;

        //std::cout<<selstart.glyph<<"\t"<<selstart.byte<<"\t|\t"<<sellen.glyph<<"\t"<<sellen.byte<<"\t|\t"<<glyphcount<<std::endl;

        Geometry::Point location;
        
        if(display == "") {
            location = {0, 0};
        }
        else {
            location = renderer.GetPosition(display, stack.TagBounds(UI::ComponentTemplate::ContentsTag).Width(), pos).TopLeft();
        }
        stack.SetTagLocation(UI::ComponentTemplate::CaretTag, location);
        
        if(stack.IndexOfTag(UI::ComponentTemplate::SelectionTag) != -1) {
            auto selbounds = stack.TagBounds(UI::ComponentTemplate::SelectionTag);
            if(sellen.byte == 0) {
                stack.SetTagSize(UI::ComponentTemplate::SelectionTag, {0, selbounds.Height()});
            }
            else {
                auto srclocation = renderer.GetPosition(display, stack.TagBounds(UI::ComponentTemplate::ContentsTag).Width(), selstart.glyph).BottomLeft();
                
                if(srclocation.X < location.X) {
                    stack.SetTagLocation(UI::ComponentTemplate::SelectionTag, {srclocation.X, 0});
                    stack.SetTagSize(UI::ComponentTemplate::SelectionTag, {location.X - srclocation.X, selbounds.Height()});
                }
                else {
                    stack.SetTagLocation(UI::ComponentTemplate::SelectionTag, {location.X, 0});
                    stack.SetTagSize(UI::ComponentTemplate::SelectionTag, {srclocation.X - location.X, selbounds.Height()});
                }
            }
        }
    }


    void Inputbox_base::focused() {
        UI::ComponentStackWidget::focused();
        
        updateselection();
    }


    
    void Inputbox_base::mousedown(UI::ComponentTemplate::Tag, Geometry::Point location, Input::Mouse::Button button) { 
        if(button != Input::Mouse::Button::Left) {
            return;
        }
        
        ismousedown = true;
        
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
        
        auto bounds = stack.BoundsOf(idx);
        
        location -= bounds.TopLeft();
        
        selstart.glyph = renderer.GetCharacterIndex(display, bounds.Width(), location);
        selstart.byte  = 0;
        int g = selstart.glyph;
        pglyph = g;
        
        while(g) {
            selstart.byte += String::UTF8Bytes(display[selstart.byte]);
            g--;
        }
        
        sellen = {0,0};
        
        updateselection();
    }
    
    void Inputbox_base::mousemove(UI::ComponentTemplate::Tag, Geometry::Point location) { 
        if(!ismousedown) {
            return;
        }
        
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
        
        auto bounds = stack.BoundsOf(idx);
        
        location -= bounds.TopLeft();
        
        int glyph = renderer.GetCharacterIndex(display, bounds.Width(), location);
        
        if(glyph == pglyph)
            return;
        
        pglyph = glyph;
        
        int byte  = 0;
        int g = glyph;
        while(g) {
            byte += String::UTF8Bytes(display[byte]);
            g--;
        }
        
        sellen.glyph = glyph - selstart.glyph;
        sellen.byte  = byte  - selstart.byte;
        
        updateselection();
    }
    
    void Inputbox_base::mouseup(UI::ComponentTemplate::Tag, Geometry::Point, Input::Mouse::Button button) { 
        if(button == Input::Mouse::Button::Left) {
            ismousedown = false;
        }
    }
    
} } }
