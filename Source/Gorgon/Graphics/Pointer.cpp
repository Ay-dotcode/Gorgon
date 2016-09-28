#include "Pointer.h"

namespace Gorgon { namespace Graphics {

    void PointerStack::Add(PointerType type, const Pointer &pointer) {
        ASSERT((int)type>(int)PointerType::None && (int)type<=(int)PointerType::Drag, "Invalid pointer type");
        
        if(pointers[(int)type].ptr && pointers[(int)type].owned) {
            delete pointers[(int)type].ptr;
        }
        
        pointers[(int)type].ptr   = &pointer;
        pointers[(int)type].owned = false;
    }
    
    void PointerStack::Assume(PointerType type, const Pointer &pointer) {
        Add(type, pointer);
        pointers[(int)type].owned = true;
    }
    
    void PointerStack::Add(PointerType type, const Drawable &image, Geometry::Point hotspot) {
        auto ptr = new Pointer(image, hotspot);
        
        Add(type, *ptr);
        pointers[(int)type].owned = true;
    }

    PointerStack::Token PointerStack::Set(PointerType type) {
        ASSERT((int)type>(int)PointerType::None && (int)type<=(int)PointerType::Drag, "Invalid pointer type");
       
        if(!pointers[(int)type].ptr) return Token();
        
        stack.Add(lastind, pointers[(int)type].ptr);
        
        return Token(this, lastind++);
    }
    
    PointerStack::Token PointerStack::Set(const Pointer &pointer) {
        stack.Add(lastind, pointer);
        
        return Token(this, lastind++);
    }
    
    void PointerStack::Reset(Token &token) {
        if(token.parent != this) return;
        
        stack.Remove(token.ind);
        
        token.parent = nullptr;
        token.ind = 0;
    }
    
    const Pointer &PointerStack::Current() const {
        if(stack.GetSize() > 0) {
            return stack.Last().Current().second;
        }
        else {
            for(int i=(int)PointerType::Arrow; i<=(int)PointerType::Drag; i++) {
                if(pointers[i].ptr)
                    return *pointers[i].ptr;
            }
        }
        
        throw std::runtime_error("No suitable pointer found.");
    }
    
    bool PointerStack::IsValid() const {
        if(stack.GetSize() > 0) {
            return true;
        }
        else {
            for(int i=(int)PointerType::Arrow; i<=(int)PointerType::Drag; i++) {
                if(pointers[i].ptr)
                    return true;
            }
        }
        
        return false;
    }
    
} }
