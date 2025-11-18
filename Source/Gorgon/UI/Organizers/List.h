#pragma once

#include "Base.h"

namespace Gorgon { namespace UI { namespace Organizers {
    
    /**
     * List organizer will organize components one per line
     * like a list. It will also set the widths of every
     * widget to the usable width of their container
     */
    class List : public Base {
    public:
        /// Constructs a new list organizer specifying spacing between lines
        explicit List(int spacing) : usedefaultspacing(false), spacing(spacing) {
        }
        
        /// Constructs a new list organizer, spaces between lines are obtained
        /// from the active parent 
        List() = default;
        
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
        
    protected:
        virtual void reorganize() override;
        
        bool usedefaultspacing = true;
        int spacing = 0;
    };
    
} } }
