#pragma once

#include "Base.h"
#include "Animation.h"
#include "../Graphics/Pointer.h"
#include "../Graphics/Drawables.h"
#include "../Graphics/Bitmap.h"
#include "../Graphics/Animations.h"
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
    class Pointer : public Base, public Graphics::AnimationProvider {
    public:
        Pointer(Graphics::Bitmap &bmp, Geometry::Point hotspot, Graphics::PointerType type);

        Pointer(Graphics::ImageAnimationProvider &anim, Geometry::Point hotspot, Graphics::PointerType type);
        
        Pointer();
        
        Pointer(const Pointer &) = delete;
        
        Pointer &operator =(const Pointer &) = delete;
        
        virtual ~Pointer();
        
        void Set(Graphics::Bitmap &bmp, Geometry::Point hotspot, Graphics::PointerType type);
        
        void Set(Graphics::ImageAnimationProvider &anim, Geometry::Point hotspot, Graphics::PointerType type);
        
        void Assume(Graphics::Bitmap &bmp, Geometry::Point hotspot, Graphics::PointerType type);
        
        void Assume(Graphics::ImageAnimationProvider &anim, Geometry::Point hotspot, Graphics::PointerType type);
        
        void Remove();
        
        Graphics::Pointer CreatePointer() const;
        
        Graphics::ImageAnimation CreateAnimation(Animation::Timer &timer) const;
        
		/// This function loads a bitmap font resource from the given file
		static Pointer *LoadResource(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long size);
    };

    
} }
