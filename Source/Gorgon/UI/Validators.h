#pragma once

#include <string>
#include "../String.h"

namespace Gorgon { namespace UI {
    
    /**
     * 
     * @page validators Validation
     * 
     * TODO
     * 
     */
    
    /**
     * Accepts all input and tries to convert using in-library functions
     */
    template<class T_>
    class EmptyValidator {
    public:
        
        /// Checks if the given string is valid
        bool IsValid(std::string) const {
            return true;
        }
        
        /// Checks if given string can be inserted between start and end
        bool AllowInsert(std::string /*start*/, std::string /*insert*/, std::string /*end*/) const {
            return true;
        }
        
        /// Checks if given number of characters can be erased from before.
        bool AllowErase(std::string /*before*/, int /*count*/, std::string /*after*/) const {
            return true;
        }
        
        /// Checks if given number of characters can be replace with insert at the end of before.
        bool AllowReplace(std::string /*before*/, int /*count*/, std::string /*insert*/, std::string /*after*/) const {
            return true;
        }
        
        /// Converts the given string to the type. If input is not valid, return initial value
        T_ From(std::string text) const {
            return String::To<T_>(text);
        }
        
        /// Converts the given value to string.
        std::string ToString(const T_ &value) {
            return String::From(value);
        }
        
    };
    
} }
