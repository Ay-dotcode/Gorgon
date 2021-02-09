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
     */
    class Flow : public Base {
    public:
        /// Is used to mark line breaks
        class BreakTag {
        };
        
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
        
        /// Inserts a line break after the widget at the given index.
        /// Reordering widgets do not automatically arrange breaks.
        /// Multiple breaks will leave unit width space between the lines.
        /// -1 will add a space at the front.
        void InsertBreak(int index) {
            breaks.insert(index);
            Reorganize();
        }
        
        /// Inserts a line break after the given widget. This only works
        /// if the widget is currently in the container.
        /// Reordering widgets do not automatically arrange breaks.
        /// Multiple breaks will leave unit width space between the lines.
        void InsertBreak(const Widget &widget);
        
        /// Removes the break at the given index. If it does not exist,
        /// nothing will be done. If there are multiple breaks at the
        /// same point, all of them will be removed.
        void RemoveBreak(int index) {
            breaks.erase(index);
            Reorganize();
        }
        
        /// Returns the number of breaks at the given index.
        int BreakCount(int index) const {
            return (int)breaks.count(index);
        }
        
        /// Removes all breaks in the organizer
        void RemoveAllBreaks() {
            breaks.clear();
            Reorganize();
        }
        
        /// Adds the given widget to the attached container.
        virtual Flow &operator << (Widget &widget) override;
        
        /// Adds the given text as a label to the attached container
        virtual Flow &operator << (const std::string &title) override {
            Base::operator <<(title);
            
            return *this;
        }
        
        /// When supplied with std::endl, inserts a line break.
        Flow &operator << (std::ostream &(*fn)(std::ostream &));
        
        /// Inserts a line break.
        Flow &operator << (BreakTag) {
            InsertBreak();
            
            return *this;
        }
        
        /// Changes the alignment of the widgets if the line is not full
        Flow &operator << (Graphics::TextAlignment alignment);
        
        /// Sets the size of the next widget in unit sizes
        Flow &operator << (int unitsize) {
            nextsize = unitsize;
            
            return *this;
        }
        
        /// Adds a button to the container
        Flow &operator << (const std::pair<std::string, std::function<void()>> &action);
        
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
        
        bool usedefaultspacing = true;
        int spacing = 0;
        bool tight = false;
        int nextsize = -1;
        std::unordered_multiset<int> breaks;
        std::unordered_map<int, Graphics::TextAlignment> aligns;
        Graphics::TextAlignment defaultalign = Graphics::TextAlignment::Left;
    };
    
} } }
