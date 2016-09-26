#pragma once

#include "Drawables.h"
#include "../Utils/Assert.h"
#include "../Geometry/Point.h"
#include "../Containers/Hashmap.h"


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

		~Pointer();
        
        
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

    class PointerStack {
    public:
        
        /// Token type
        using Token = int;
        
        /// Null token, if this token is returned this means there is no
        /// pointer registered for the given type. If this token is given
        /// to Reset function, the function will take no action.
		static const Token NullToken = 0;

		///Pointer types
		enum PointerType {
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
        
        /// Adds the given pointer to the stack. Ownership of the pointer will not
        /// be transferred. If the given pointer type exists old one will be overriden.
        /// If the old pointer is managed by this stack then it will be deleted.
        void Add(PointerType type, const Pointer &pointer);
        
        /// Adds the given pointer to the stack. Ownership of the pointer will be
        /// transferred. If the given pointer type exists old one will be overriden.
        /// If the old pointer is managed by this stack then it will be deleted.
        void Assume(PointerType type, const Pointer &pointer);
        
        /// Creates and adds a new pointer. Life time of this new pointer will be
        /// bound to the life time of the stack. If the given pointer type exists
        /// old one will be overriden. If the old pointer is managed by this stack
        /// then it will be deleted.
        void Add(PointerType type, const Drawable &image, Geometry::Point hotspot);
        
        /// Set the current pointer to the given type. This would return a token to
        /// be used to reset this operation.
		Token Set(PointerType Type);
        
        /// Sets the current pointer in the stack to the given pointer. This pointer
        /// will not be added to the list.
        Token Set(const Pointer &pointer);
        
        /// Removes a pointer shape from the stack. If a NullToken is given, this 
        /// function will not perform any action
        void Reset(Token token);
        
        /// Sets the base pointer, which will be used after all sets are reset.
        void ChangeBase(Pointer &Pointer);
        
    private:
        struct Wrapper {
            Pointer *ptr = nullptr;
            bool owned = false;
        };
        
        int lastind = 0;
        
        Containers::Hashmap<int, Pointer> stack;
        
        std::array<Wrapper, Drag+1> pointers;
    };
    

} }
