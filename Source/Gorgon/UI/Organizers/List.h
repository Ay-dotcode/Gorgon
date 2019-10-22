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
        /// Constructs a new list organizer, spacing between lines can be
        /// specified.
        explicit List(int spacing = 5) : spacing(spacing) {
        }
        
        /// Sets the spacing between the lines
        void SetSpacing(int value) {
            spacing = value;
        }
        
        /// Returns the spacing between the lines
        int GetSpacing() const {
            return spacing;
        }
        
    protected:
        virtual void reorganize() override;
        
        int spacing;
    };
    
} } }
