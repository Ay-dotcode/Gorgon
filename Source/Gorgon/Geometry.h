#pragma once

#include "Scripting/Reflection.h"

namespace Gorgon { namespace Geometry {

    extern Scripting::Library LibGeometry;
    
    namespace Types {
#define DEFTYPE(name) \
        inline const Scripting::Type &name() { \
            static const Scripting::Type *type = LibGeometry.GetType(#name); \
            return *type; \
        }

        DEFTYPE(Point)
        DEFTYPE(Pointf)
        DEFTYPE(Size)
        DEFTYPE(Sizef)
        
#undef DEFTYPE
    }
    
    void InitializeScripting();
    
} }
