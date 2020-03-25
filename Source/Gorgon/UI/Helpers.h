#pragma once

namespace Gorgon { namespace UI {
   
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
    
} }
