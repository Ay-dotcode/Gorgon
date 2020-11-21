#pragma once

#include "Base.h"

#include <unordered_set>

namespace Gorgon { namespace UI { 

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
        
        /// Sets tight arrangement
        void SetTight(bool value) {
            if(value == tight)
                return;
            
            tight = value;
            
            if(IsAttached())
                reorganize();
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
            return breaks.count(index);
        }
        
        /// Removes all breaks in the organizer
        void RemoveAllBreaks() {
            breaks.clear();
            Reorganize();
        }
        
    protected:
        virtual void reorganize() override;
        
        bool usedefaultspacing = true;
        int spacing = 0;
        bool tight = false;
        std::unordered_multiset<int> breaks;
    };
    
} } }
