#pragma once

#include "Base.h"
#include "Animation.h"
#include "../Graphics/Pointer.h"
#include "../Graphics/Drawables.h"
#include "../Graphics/Bitmap.h"
#include "../Graphics/Animations.h"
#include "../Graphics/ImageAnimation.h"
#include "../Utils/Assert.h"
#include "../Geometry/Point.h"


namespace Gorgon { namespace Resource {

    class File;
	class Reader;

    /**
     * Pointer resource that can be used to create a new pointer to be
     * displayed. A Pointer resource can be created from a bitmap or an
     * animation.
     */
    class Pointer : public Base, public Graphics::BitmapAnimationProvider, public Graphics::PointerProvider {
    public:
        Pointer(Graphics::Bitmap &bmp, Geometry::Point hotspot, Graphics::PointerType type) : 
        Graphics::PointerProvider(dynamic_cast<Graphics::BitmapAnimationProvider&>(*this), hotspot), type(type) { 
            Add(bmp);
        }

        Pointer(Graphics::BitmapAnimationProvider &&anim, Geometry::Point hotspot, Graphics::PointerType type) :
        Graphics::BitmapAnimationProvider(std::move(anim)),
        Graphics::PointerProvider(dynamic_cast<Graphics::BitmapAnimationProvider&>(*this), hotspot), type(type) { 
            
        }
        
        explicit Pointer(Graphics::PointerType type = Graphics::PointerType::None) : Graphics::PointerProvider(dynamic_cast<Graphics::BitmapAnimationProvider&>(*this)),
        type(type) { 
            
        }
        
        Pointer(const Pointer &) = delete;
        
        Pointer &operator =(const Pointer &) = delete;
        
        GID::Type GetGID() const override { return GID::Pointer; }
        
        /// Returns the type of the pointer
        Graphics::PointerType GetType() const {
            return type;
        }
        
        /// Sets the type of the pointer
        void SetType(Graphics::PointerType value) {
            type = value;
        }
        
        /// Moves the pointer provider out of resource system. Use Prepare and Discard before calling this function to
        /// avoid data duplication
        Graphics::PointerProvider MoveOut();
        
		/// This function loads a bitmap font resource from the given file
		static Resource::Pointer* LoadResource(std::weak_ptr< Gorgon::Resource::File > file, std::shared_ptr< Gorgon::Resource::Reader > reader, long unsigned int size);
		
		/// This function loads a bitmap font resource from the given file
		static Pointer *LoadLegacy(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long size) { Utils::NotImplemented(); }
        
    protected:
        Graphics::PointerType type = Graphics::PointerType::Arrow;
         
        virtual ~Pointer() { }
		
		void save(Writer &writer) const override;
    };

    
} }
