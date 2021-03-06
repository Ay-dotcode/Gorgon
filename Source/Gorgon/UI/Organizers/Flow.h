#pragma once

#include "Base.h"
#include "../../Graphics.h"

#include <iosfwd>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <functional>

namespace Gorgon { 

namespace Widgets {
    
    class Button;
    
}
    
namespace UI { 

    class Widget;
    
namespace Organizers {
    
    /** 
     * Flow organizer places widgets side by side until they won't
     * fit in the same row any more. In that case it will flow to
     * next line. If tight option is not set, rows will have the
     * height of the highest widget. If it is set, next widget is
     * placed as close as possible to the widget above. This will
     * also mean there could be widgets wrapping around a longer
     * one.
     * 
     * Important: Flow organizer will work best if unit sizes are
     * used.
     * 
     * @warning Removing widgets will throw off modifiers and breaks.
     * Instead of removing them, you may hide widgets.
     */
    class Flow : public Base { 
    public:
        /// Is used to mark line breaks
        class BreakTag {
        };
        
    private:
        //contains all alignments
        class Modifier {
        public:
            enum Type {
                Break,
                Align,
                HSpace,
                VSpace,
            };
            
            Modifier(BreakTag) : type(Break) {
                
            }
            
            Modifier(Graphics::TextAlignment align) : type(Align), align(align) { }
            
            Modifier(Type type, int size) : type(type), size(size) { }
            
            Type type;
            
            union {
                Graphics::TextAlignment align;
                int size;
            };
            
        };
        
        class Flower {
            friend class Flow;
        public:
            
            ~Flower() {
                if(base)
                    base->StartReorganize();
            }
            
            /// Adds the given widget to the attached container.
            virtual Flower &operator << (Widget &widget) {
                base->flow(widget);
                
                return *this;
            }
            
            /// Adds the given text as a label to the attached container
            virtual Flower &operator << (const std::string &title) {
                base->flow(title);
                
                return *this;
            }
            
            /// When supplied with std::endl, inserts a line break.
            Flower &operator << (std::ostream &(*fn)(std::ostream &)) {
                base->flow(fn);
                
                return *this;
            }
            
            /// Inserts a line break.
            Flower &operator << (BreakTag tag) {
                base->flow(tag);
                
                return *this;
            }
                    
            /// Changes the alignment of the widgets if the line is not full
            Flower &operator << (Graphics::TextAlignment alignment) {
                base->flow(alignment);
                
                return *this;
            }
            
            /// Sets the size of the next widget in unit sizes
            Flower &operator << (int size) {
                base->flow(size);
                
                return *this;
            }
            
            /// Adds a button to the container
            Flower &operator << (const std::pair<std::string, std::function<void()>> &action) {
                base->flow(action);
                
                return *this;
            }

        private:
            Flower(Flow *base) : base(base) { 
                base->PauseReorganize();
            }
            
            Flower(const Flower &) = delete;
            
            Flower(Flower &&other) : base(other.base) {
                other.base = nullptr;
            }

            Flow *base = nullptr;
        };
        
        friend class Flower;
    public:
        /// Constructs a new flow organizer specifying spacing between widgets
        explicit Flow(int spacing) : usedefaultspacing(false), spacing(spacing) {
        }
        
        /// Constructs a new list organizer, spaces between widgets are obtained
        /// from the active parent
        Flow() = default;
        
        /// Sets the spacing between the lines
        void SetSpacing(int value) {
            if(value == spacing && !usedefaultspacing)
                return;
            
            usedefaultspacing = false;
            spacing = value;
            
            if(IsAttached())
                reorganize();
        }
        
        /// Starts using default spacing.
        void UseDefaultSpacing() {
            if(usedefaultspacing)
                return;
            
            usedefaultspacing = true;
            
            if(IsAttached())
                reorganize();
        }
        
        /// Returns the spacing between the lines
        int GetSpacing() const;
        
        /// Sets tight arrangement, not working yet
        void SetTight(bool value) {
            if(value == tight)
                return;
            
            tight = value;
            
            if(IsAttached())
                reorganize();
        }
        
        /// Changes the default alignment of the widgets. Alignment of a specific line can be altered
        /// by streaming Flow::Left, Center and Right.
        void SetAlignment(Graphics::TextAlignment value);
        
        /// Returns the default alignment of the widgets.
        Graphics::TextAlignment GetAlignment() const {
            return defaultalign;
        }
        
        /// Inserts a line break after the last widget. Reordering
        /// widgets do not automatically arrange breaks. Multiple breaks
        /// will leave unit width space between the lines.
        void InsertBreak();
        
        /// Inserts a line break before the widget at the given index.
        /// Reordering widgets do not automatically arrange breaks.
        /// Multiple breaks will leave unit width space between the lines.
        /// -1 will add a space at the front.
        void InsertBreak(int index);
        
        /// Inserts a line break after the given widget. This only works
        /// if the widget is currently in the container.
        /// Reordering widgets do not automatically arrange breaks.
        /// Multiple breaks will leave unit width space between the lines.
        void InsertBreak(const Widget &widget);
        
        /// Removes all modifiers such as breaks in the organizer
        void RemoveAllModifiers() {
            modifiers.clear();
            Reorganize();
        }
        
        /// This will create a modifier object that should be inserted into ui stream
        Modifier HSpace(int unitsize) {
            return {Modifier::HSpace, unitsize};
        }
        
        /// This will create a modifier object that should be inserted into ui stream
        Modifier VSpace(int spaces) {
            return {Modifier::VSpace, spaces};
        }
        
        virtual Flow &Add(const std::string &title) override {
            Base::Add(title);
            
            return *this;
        }
        
        virtual Flow &Add(Widget &widget) override;
        
        /// Adds the given widget to the attached container.
        Flower operator << (Widget &widget) {
            return std::move(Flower(this) << widget);
        }
        
        /// Adds the given text as a label to the attached container
        Flower operator << (const std::string &title) {
            return std::move(Flower(this) << title);
        }
        
        /// When supplied with std::endl, inserts a line break.
        Flower operator << (std::ostream &(*fn)(std::ostream &)) {
            return std::move(Flower(this) << fn);
        }
        
        /// Inserts a line break.
        Flower operator << (BreakTag tag) {
            return std::move(Flower(this) << tag);
        }
        
        /// Changes the alignment of the widgets if the line is not full
        Flower operator << (Graphics::TextAlignment alignment) {
            return std::move(Flower(this) << alignment);
        }
        
        /// Sets the size of the next widget in unit sizes
        Flower operator << (unsigned size) {
            return std::move(Flower(this) << size);
        }
        
        /// Adds a button to the container
        Flower operator << (const std::pair<std::string, std::function<void()>> &action) {
            return std::move(Flower(this) << action);
        }
        
        /// Creates an action, which when streamed to organizer will create a button
        std::pair<std::string, std::function<void()>> Action(const std::string &text, std::function<void()> fn) {
            return std::make_pair(text, fn);
        }
        
        /// Creates an action, which when streamed to organizer will create a button
        template<class T_>
        std::pair<std::string, std::function<void()>> Action(const std::string &text, T_ &obj, void(T_::*fn)()) {
            return std::make_pair(text, std::bind(fn, obj));
        }
        
        /// Creates an action, which when streamed to organizer will create a button
        template<class T_>
        std::pair<std::string, std::function<void()>> Action(const std::string &text, T_ *obj, void(T_::*fn)()) {
            return std::make_pair(text, std::bind(fn, obj));
        }
        
        static BreakTag Break;
        static Graphics::TextAlignment Left, Center, Right;
        
    protected:
        virtual void reorganize() override;
        
        void flow(Widget &widget) {
            Add(widget);
        }
        
        void flow(const std::string &title);
        
        void flow(Modifier modifier);
        
        void flow(BreakTag);
        
        void flow(unsigned size) {
            nextsize = size;
        }
        
        void flow(std::ostream &(*fn)(std::ostream &));
        
        void flow(Graphics::TextAlignment alignment);
        
        void flow(const std::pair<std::string, std::function<void()>> &action);
        
        bool usedefaultspacing = true;
        int spacing = 0;
        bool tight = false;
        int nextsize = -1;
        std::unordered_multimap<int, Modifier> modifiers;
        Graphics::TextAlignment defaultalign = Graphics::TextAlignment::Left;
    };
    
} } }
