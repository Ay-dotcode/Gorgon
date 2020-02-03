#pragma once

#include <functional>
#include <string>

#include "../Event.h"
#include "../Property.h"
#include "../UI/Validators.h"
#include "../UI/ComponentStackWidget.h"
#include "../Input/KeyRepeater.h"

namespace Gorgon { namespace Widgets {
    
    /// @cond internal
    namespace internal {
        template<class I_, class T_>
        struct prophelper {
			prophelper(I_ *obj) : obj(obj) {}

            ~prophelper() { 
                
            }
            
            void set_(const T_&v) {
                obj->set(v);
            }
            T_ get_() const {
                return obj->get();
            }

			I_ *obj;
        };
    }
    /// @endcond
    
    /**
     * This class allows users to enter any value to an inputbox. This
     * class is specialized as Textbox and Numberbox. It is possible to
     * supply a validator to a specific inputbox. Inputbox is designed
     * for value objects that can be copied and serialized to string.
     */
    template<class T_, class V_ = UI::EmptyValidator<T_>, template<class C_, class PT_, PT_(C_::*Getter_)() const, void(C_::*Setter_)(const PT_ &)> class P_ = Gorgon::Property>
    class Inputbox : 
        public UI::ComponentStackWidget, 
        public P_<internal::prophelper<Inputbox<T_, V_, P_>, T_>, T_, &internal::prophelper<Inputbox<T_, V_, P_>, T_>::get_, &internal::prophelper<Inputbox<T_, V_, P_>, T_>::set_> {

        //for keeping selection both in bytes and glyphs
        struct glyphbyte {
            int glyph, byte;

            glyphbyte &operator +=(const glyphbyte &other) {
                glyph += other.glyph;
                byte += other.byte;

                return *this;
            }

            glyphbyte operator +(const glyphbyte &other) const {
                return {glyph + other.glyph, byte + other.byte};
            }
        };

        static constexpr int allselected = std::numeric_limits<int>::max();

	public:
        
        using Type     = T_;
        using PropType = P_<internal::prophelper<Inputbox<T_, V_, P_>, T_>, T_, &internal::prophelper<Inputbox<T_, V_, P_>, T_>::get_, &internal::prophelper<Inputbox<T_, V_, P_>, T_>::set_>;

        friend class P_<internal::prophelper<Inputbox<T_, V_, P_>, T_>, T_, &internal::prophelper<Inputbox<T_, V_, P_>, T_>::get_, &internal::prophelper<Inputbox<T_, V_, P_>, T_>::set_>;
		template<class T1_, class V1_, template<class C_, class PT_, PT_(C_::*Getter_)() const, void(C_::*Setter_)(const PT_&)> class P1_>
		friend class Inputbox;
		friend struct internal::prophelper<Inputbox<T_, V_, P_>, T_>;
        
        /// Initializes the inputbox
        explicit Inputbox(const UI::Template &temp, T_ value = T_()) : 
        ComponentStackWidget(temp), PropType(&helper), value(value), display(validator.ToString(value))
        {
            
            stack.HandleMouse(Input::Mouse::Button::Left);
            
            stack.SetMouseDownEvent([this](auto, auto, auto btn) {
                if(allowfocus() && btn == Input::Mouse::Button::Left)
                    Focus();
            });
            
            updatevalue();
            updateselection();

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
        
        /// Initializes the inputbox
        explicit Inputbox(const UI::Template &temp, std::function<void()> changedevent) : Inputbox(temp) {
            ChangedEvent.Register(changedevent);
        }
        
        /// Initializes the inputbox
        explicit Inputbox(const UI::Template &temp, T_ value, std::function<void()> changedevent) : Inputbox(temp, value)
        {
            ChangedEvent.Register(changedevent);
        }
        
        /// Assignment to the value type
        Inputbox &operator =(const T_ value) {
            set(value);
            
            return *this;
        }
        
        /// Copy assignment will only copy the value
        Inputbox &operator =(const Inputbox &value) {
            set(value.Get());
            
            return *this;
        }
        
        
        /// Returns the value in the box.
        operator T_() const {
            return value;
        }
        
        /// Returns the length of the text in this inputbox. This value is in glyphs and takes time to calculate.
        int Length() const {
            return glyphcount;
        }
        
        /// Returns the current text in the inputbox. Use Get() to obtain the value of
        /// the inputbox, this function exists for special uses.
        std::string GetText() const {
            return display;
        }
        
        /// @name Selection
        /// @{
        
        /// Selects the entire contents of this inputbox
        void SelectAll() {
            selstart = {0, 0};
            sellen = {allselected, allselected};
            updateselection();
        }
        
        /// Removes any selection, does not move the start of the
        /// cursor.
        void SelectNone() {
            sellen = {0, 0};
            updateselection();
        }
        
        /// Returns the start of the selection in glyphs.
        int SelectionStart() const {
            return selstart.glyph;
        }
        
        /// Returns the length of the selection in glyphs. Selection length could be 
        /// negative denoting the selection is backwards.
        int SelectionLength() const {
            return sellen.glyph;
        }
        
        /// Returns the location of the caret. It is always at the end of the selection.
        int CaretLocation() const {
            return selstart.glyph + sellen.glyph;
        }
        
        /// @}
        
        bool Activate() override {
            return Focus();
        }
        
        bool CharacterEvent(Gorgon::Char c) override {
            if(sellen.byte != 0) {
                if(sellen.byte == allselected) {
                    selstart.byte += (int)display.length();
                    selstart.glyph += Length();
                }
                else {
                    selstart += sellen;
                }
            }

            String::AppendUnicode(display, c);
            glyphcount++;

            selstart.glyph++;
            selstart.byte += String::UnicodeUTF8Bytes(c);

            sellen = {0, 0};
            
            value = validator.From(display);
            updatevalue(false);
            updateselection();
            
            return true;
        }

        virtual bool KeyEvent(Input::Key key, float state) override { 
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

                                updatevalue(false);
                                updateselection();
                            }

                        }
                        else {
                            eraseselected();
                        }

                         return true;
                    case Keycodes::Delete: 
                        if(sellen.byte == 0) {
                            if(selstart.glyph < glyphcount) {
                                display.erase(selstart.byte, String::UTF8Bytes(display[selstart.byte]));
                                glyphcount--;

                                updatevalue(false);
                            }
                        }
                        else {
                            eraseselected();
                        }

                        return true;
                    }
                }
                else if(Input::Keyboard::CurrentModifier == Input::Keyboard::Modifier::Ctrl) {
                    switch(key) {
                    case Keycodes::A:
                        SelectAll();

                        return true;

                    }
                }
            }

            return false;
        }
        
        /// Fired after the value of the inputbox is changed. Parameter is the previous 
        /// value before the change. If the user is typing, this event will be fired
        /// after typing stops for a set amount of time. ChangedEvent will be fired
        /// immediately when a value is pasted to the box.
        Event<Inputbox, const T_ &> ChangedEvent = Event<Inputbox, const T_ &>{this};
        
        
    protected:
        
        /// updates the selection display
        void updateselection() {
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

            auto pos = selstart.glyph;

            if(sellen.glyph == allselected)
                pos = (int)display.length(); //this is ok, bytelen >= glyphlen
            else
                pos += sellen.glyph;

            std::cout<<selstart.glyph<<"\t"<<selstart.byte<<"\t|\t"<<sellen.glyph<<"\t"<<sellen.byte<<"\t|\t"<<pos<<std::endl;

            auto location = renderer.GetPosition(display, stack.TagBounds(UI::ComponentTemplate::ContentsTag).Width(), pos);
            stack.SetTagLocation(UI::ComponentTemplate::CaretTag, location.TopLeft());
        }
        
        /// updates the value display
        void updatevalue(bool updatedisplay = true) {
            if(updatedisplay) {
                display = validator.ToString(value);
                glyphcount = String::UnicodeGlyphCount(display);
            }
            
            stack.SetData(UI::ComponentTemplate::Text, display);
        }
        
        /// Returns the value in the box
        T_ get() const {
            return value;
        }
        
        /// Changes the value in the box
        void set(const T_ &val) {
            if(value == val)
                return;
            
            value = val;
            
            updatevalue();
            updateselection();
        }

        Input::KeyRepeater repeater;
        
        
    private:
        void moveselleft() {
            if(selstart.glyph > 0) {
                selstart.glyph--;
                selstart.byte--;

                //if previous byte is unicode continuation point, go further before
                while(selstart.byte && (display[selstart.byte] & 0b11000000) == 0b10000000)
                    selstart.byte--;
            }
        }

        void moveselright() {
            if(selstart.glyph < glyphcount) {
                selstart.glyph++;
                selstart.byte += String::UTF8Bytes(display[selstart.byte]);
            }
        }

        void eraseselected() {
            if(sellen.byte < 0) {
                int pos = selstart.byte + sellen.byte;

                display.erase(pos, -sellen.byte);

                glyphcount -= sellen.glyph;
                selstart += sellen;
                sellen = {0, 0};

                updatevalue(false);
                updateselection();
            }
            else if(sellen.byte > 0) {
                int pos = selstart.byte;

                display.erase(pos, sellen.byte);

                glyphcount -= sellen.glyph;
                sellen = {0, 0};

                updatevalue(false);
                updateselection();
            }
        }

        V_ validator;
        T_ value;
        std::string display;

        glyphbyte selstart = {0, 0};
        glyphbyte sellen   = {0, 0};
        int glyphcount = 0;

		struct internal::prophelper<Inputbox<T_, V_, P_>, T_> helper = internal::prophelper<Inputbox<T_, V_, P_>, T_>(this);
    };
    
} }
