#pragma once

#include "Drawables.h"
#include "../Utils/Assert.h"
#include "../Geometry/Point.h"
#include "../Containers/Hashmap.h"
#include "Animations.h"


namespace Gorgon { namespace Graphics {

    /**
     * Represents a pointer. When drawn, pointer will move the image so that the hotspot
     * will be on the given point. If desired, ownership of the image can be transferred
     * to the pointer.
     */
	class Pointer : public Drawable {
	public:
        
        Pointer() = default;

		/// Initializes a pointer. Ownership of the drawable is not transferred. Use Assume function 
        /// after invoking default constructor to transfer ownership.
		Pointer(const Drawable &image, int x, int y) : Pointer(image, {x, y}) { }

		/// Initializes a pointer. Ownership of the drawable is not transferred. Use Assume function 
        /// after invoking default constructor to transfer ownership.
		Pointer(const Drawable &image, Geometry::Point hotspot) :
		image(&image), hotspot(hotspot) {
		}
		
		Pointer(const Pointer &other) = delete;
        
        Pointer(Pointer &&other) : 
        image(other.image), hotspot(other.hotspot), owner(other.owner)
        {
            other.image = nullptr;
            other.owner = false;
        }
        
        Pointer &operator =(const Pointer &) = delete;

        Pointer &operator =(Pointer &&other) {
            RemoveImage();
            image   = other.image;
            owner   = other.owner;
            hotspot = other.hotspot;
            
            other.image = nullptr;
            other.owner = false;
            
            return *this;
        }

		//Pointer(Resource::Pointer &pointer);

		~Pointer() {
            RemoveImage();
        }
        
        
        /// Returns if the pointer has an image
        bool HasImage() const {
            return image != nullptr;
        }
        
        /// Returns the image contained in this pointer. You should check HasImage
        /// before accessing to the image
        const Drawable &GetImage() const {
            ASSERT(image, "Pointer image is not set");
            
            return *image;
        }
        
        /// Changes the image of this pointer 
        void SetImage(const Drawable &value) {
            RemoveImage();

            image = &value;
        }
        
        /// Changes the image of the pointer by assuming the ownership of the given
        /// image
        void Assume(const Drawable &value) {
            RemoveImage();

            image = &value;
            owner = true;
        }
        
        /// Removes the image from the pointer
        void RemoveImage() {
            if(owner) {
                delete image;
                owner = false;
            }
            
            image = nullptr;
        }
        
        /// Releases the ownership of the drawable and removes it from the pointer
        const Drawable &Release() {
            auto img = image;
            
            owner = false;
            image = nullptr;
            
            return *img;
        }
            
        
    protected:
        void draw(Gorgon::Graphics::TextureTarget &target, const Geometry::Pointf &p, Gorgon::Graphics::RGBAf color) const override {
            if(!image) return;
            
            image->Draw(target, p-hotspot, color);
        }
        
    private:        
		const Drawable *image = nullptr;
        Geometry::Point hotspot = {0, 0};
        
        bool owner = false;
	};
    
    /// This class stores information that allows a pointer to be created.
    class PointerProvider {
    public:
        PointerProvider(const AnimationProvider &provider, Geometry::Point hotspot = {0,0}, bool owned = false) : 
        provider(provider), hotspot(hotspot), owned(owned) {
        }
        
        /// Move constructor
        PointerProvider(PointerProvider &&other) : 
        provider(other.provider), hotspot(other.hotspot), owned(other.owned) {
            other.owned = false;
        }
        
        PointerProvider(const PointerProvider &) = delete;
        
        ~PointerProvider() {
            if(owned)
                delete &provider;
        }
        
        /// Creates a pointer from this provider
        Pointer CreatePointer(Gorgon::Animation::Timer &timer) const {
            return Pointer(provider.CreateAnimation(timer), hotspot);
        }
        
        /// Creates a pointer from this provider
        Pointer CreatePointer(bool create = true) const {
            return Pointer(provider.CreateAnimation(create), hotspot);
        }
        
        /// Returns the hotspot of the provider
        Geometry::Point GetHotspot() const {
            return hotspot;
        }
        
        /// Sets the hotspot of the pointer
        void SetHotspot(Geometry::Point value) {
            hotspot = value;
        }
        
    protected:
        /// Provides animation for pointer creation
        const AnimationProvider &provider;
        
        /// Hotspot will be transferred to newly created pointers
        Geometry::Point hotspot;
        
        /// Whether the animation is owned by this object
        bool owned;
    };
    
    ///Pointer types
    enum class PointerType {
        ///No pointer is selected or using default
        None=0,
        
        ///Arrow / Pointer
        Arrow=1,
        
        ///Wait / Hourglass
        Wait=2,
        
        ///No / Not allowed
        No=3,
        
        ///Text / Beam pointer
        Text=4,
        
        ///Hand pointer
        Hand=5,
        
        ///Drag / Closed hand pointer
        Drag=6
    };
        

    /**
     * This class manages a pointer stack that allows multiple pointers to be
     * registered and switched. These pointers are pushed to the stack and can
     * be reset, setting the pointer to the previous state. 
     */
    class PointerStack {
    public:
        
        /// Token type, automatically pops pointer stack when goes out of scope
        class Token {
            friend class PointerStack;
        public:
            Token() { }
            
            Token(Token &&other) {
                parent = other.parent;
                ind = other.ind;
                
                other.parent = nullptr;
                other.ind = 0;
            }
            
            ~Token() {
                if(parent)
                    parent->Reset(*this);
            }
            
            Token &operator =(Token &&tok) {
                if(parent)
                    parent->Reset(*this);
                
                parent = tok.parent;
                ind = tok.ind;
                
                tok.parent = nullptr;
                tok.ind = 0;
                
                return *this;
            }
            
            /// Checks if the token is null
            bool IsNull() const { return parent == nullptr; }
            
            /// Checks if the token is valid
            explicit operator bool() const { return !IsNull(); }
            
            /// Checks if the token is invalid
            bool operator !() const { return IsNull(); }
            
        private:
            Token(PointerStack *parent, int ind) : parent(parent), ind(ind) { }
            
            PointerStack *parent = nullptr;
            int ind = 0;
        };

		PointerStack() = default;

		PointerStack(const PointerStack &) = delete;

		PointerStack(PointerStack &&other) {
			using std::swap;

			swap(lastind, other.lastind);
			swap(stack, other.stack);
			swap(pointers, other.pointers);

		}

		~PointerStack() {
			for(auto &w : pointers) {
				if(w.owned)
					delete w.ptr;
			}
		}

        /// Adds the given pointer to the stack. Ownership of the pointer will not
        /// be transferred. If the given pointer type exists old one will be overriden.
        /// If the old pointer is managed by this stack then it will be deleted.
		void Add(PointerType type, const Pointer &pointer);

		/// Move variant that maps to assume
		void Add(PointerType type, const Pointer &&pointer) {
			Assume(type, pointer);
		}

        /// Adds the given pointer to the stack. Ownership of the pointer will be
        /// transferred. If the given pointer type exists old one will be overriden.
        /// If the old pointer is managed by this stack then it will be deleted.
        void Assume(PointerType type, const Pointer &pointer);
        
        /// Creates and adds a new pointer. Life time of this new pointer will be
        /// bound to the life time of the stack. If the given pointer type exists
        /// old one will be overridden. If the old pointer is managed by this stack
        /// then it will be deleted.
		void Add(PointerType type, const Drawable &image, Geometry::Point hotspot);

		void Add(PointerType type, const Drawable &&image, Geometry::Point hotspot) = delete;

        /// Checks if the given pointer exists
        bool Exists(PointerType type) {
            if((int)type <= 0 || (int)type > (int)PointerType::Drag) return false;
            
            return pointers[(int)type].ptr != nullptr;
        }
        
        /// Set the current pointer to the given type. This would return a token to
        /// be used to reset this operation. The token should be stored in a variable
        /// otherwise the pointer would be reset immediately.
		Token Set(PointerType type);
        
        /// Sets the current pointer in the stack to the given pointer. This pointer
        /// will not be added to the list. The token should be stored in a variable
        /// otherwise the pointer would be reset immediately.
        Token Set(const Pointer &pointer);
        
        /// Removes a pointer shape from the stack. If the given token is null or 
        /// does not belong to this stack, nothing is done.
        void Reset(Token &token);
        
        /// Returns the pointer on top of the stack, if no pointer is on the stack,
        /// first pointer in the order of PointerType enums will be returned. If no
        /// pointers are registered, this function will throw runtime_error
        const Pointer &Current() const;
        
        /// Returns if the stack is valid to be used. A valid stack requires at least
        /// one registered or pushed pointer
        bool IsValid() const;
        
    private:
        struct Wrapper {
            const Pointer *ptr = nullptr;
            bool owned = false;
        };
        
        int lastind = 0;
        
        Containers::Hashmap<int, const Pointer> stack;
        
        std::array<Wrapper, (int)PointerType::Drag+1> pointers = {};
    };
    

} }
