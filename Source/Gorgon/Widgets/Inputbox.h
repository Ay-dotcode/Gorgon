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
        
        class Inputbox_base : public UI::ComponentStackWidget {
        protected:
            Inputbox_base(const UI::Template &temp);
            
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
                
            /// Returns the length of the text in this inputbox. This value is in glyphs and takes time to calculate.
            int Length() const {
                return glyphcount;
            }
            
            /// Returns the current text in the inputbox. Use Get() to obtain the value of
            /// the inputbox, this function exists for special uses.
            std::string GetText() const {
                return display;
            }
            
            std::string GetSelectedText() const {
                if(sellen.byte < 0)
                    return display.substr(selstart.byte + sellen.byte, -sellen.byte);
                else if(sellen.byte > 0)
                    return display.substr(selstart.byte, sellen.byte);
                else
                    return "";
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
            
            bool CharacterEvent(Gorgon::Char c) override;
            
            virtual bool KeyEvent(Input::Key key, float state) override;
            
            
        protected:
            
                
            /// updates the selection display
            void updateselection();
            
            /// updates the value display
            virtual void updatevalue() = 0;
            
            /// updates the value display
            virtual void updatevaluedisplay(bool updatedisplay = true) = 0;
            
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

                    glyphcount += sellen.glyph;
                    selstart += sellen;
                    sellen = {0, 0};
                }
                else if(sellen.byte > 0) {
                    int pos = selstart.byte;

                    display.erase(pos, sellen.byte);

                    glyphcount -= sellen.glyph;
                    sellen = {0, 0};

                    updatevaluedisplay(false);
                    updateselection();
                }
            }
            
            void focused() override;
            
            void mousedown(UI::ComponentTemplate::Tag tag, Geometry::Point location, Input::Mouse::Button button);
            
            void mouseup(UI::ComponentTemplate::Tag tag, Geometry::Point location, Input::Mouse::Button button);
            
            void mousemove(UI::ComponentTemplate::Tag tag, Geometry::Point location);

            
            std::string display;

            glyphbyte selstart = {0, 0};
            glyphbyte sellen   = {0, 0};
            int glyphcount = 0;
            int pglyph;
            
            bool ismousedown = false;
        
            Input::KeyRepeater repeater;
            
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
        public internal::Inputbox_base, 
        public P_<internal::prophelper<Inputbox<T_, V_, P_>, T_>, T_, &internal::prophelper<Inputbox<T_, V_, P_>, T_>::get_, &internal::prophelper<Inputbox<T_, V_, P_>, T_>::set_> {


    public:
        
        using Type     = T_;
        using PropType = P_<internal::prophelper<Inputbox<T_, V_, P_>, T_>, T_, &internal::prophelper<Inputbox<T_, V_, P_>, T_>::get_, &internal::prophelper<Inputbox<T_, V_, P_>, T_>::set_>;

        friend class P_<internal::prophelper<Inputbox<T_, V_, P_>, T_>, T_, &internal::prophelper<Inputbox<T_, V_, P_>, T_>::get_, &internal::prophelper<Inputbox<T_, V_, P_>, T_>::set_>;
        template<class T1_, class V1_, template<class C_, class PT_, PT_(C_::*Getter_)() const, void(C_::*Setter_)(const PT_&)> class P1_>
        friend class Inputbox;
        friend struct internal::prophelper<Inputbox<T_, V_, P_>, T_>;
        
        /// Initializes the inputbox
        explicit Inputbox(const UI::Template &temp, T_ value = T_()) : 
        internal::Inputbox_base(temp), PropType(&helper), value(value)
        {
            display = validator.ToString(value);
            
            updatevaluedisplay();
            updateselection();

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
        
        /// Fired after the value of the inputbox is changed. Parameter is the previous 
        /// value before the change. If the user is typing, this event will be fired
        /// after typing stops for a set amount of time. ChangedEvent will be fired
        /// immediately when a value is pasted to the box.
        Event<Inputbox, const T_ &> ChangedEvent = Event<Inputbox, const T_ &>{this};
        
        
    protected:
        
        /// Returns the value in the box
        T_ get() const {
            return value;
        }
        
        /// Changes the value in the box
        void set(const T_ &val) {
            if(value == val)
                return;
            
            value = val;
            
            updatevaluedisplay();
            updateselection();
        }

        void updatevalue() override {
            value = validator.From(display);
            
            ChangedEvent(value);
        }
            
        /// updates the value display
        void updatevaluedisplay(bool updatedisplay = true) override {
            if(updatedisplay) {
                display = validator.ToString(value);
                glyphcount = String::UnicodeGlyphCount(display);
            }
            
            stack.SetData(UI::ComponentTemplate::Text, display);
        }
        
        
    private:
        V_ validator;
        T_ value;

        struct internal::prophelper<Inputbox<T_, V_, P_>, T_> helper = internal::prophelper<Inputbox<T_, V_, P_>, T_>(this);
    };
    
} }
