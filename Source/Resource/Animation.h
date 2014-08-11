#pragma once

#include "Base.h"
#include "../Animation.h"
#include "Image.h"

#pragma warning(push)
#pragma warning(disable:4250)

namespace Gorgon {
namespace Resource {
class File;

class Animation;

/// This class is a created image animation that can be controlled and drawn over screen.
class ImageAnimation :
    public virtual Graphics::RectangularAnimation, public virtual Graphics::Image, public virtual Graphics::TextureSource
{
public:
    /// Creates a new image animation from the given parent
    ImageAnimation(const Resource::Animation &parent, Gorgon::Animation::Timer &controller, bool owner=false);

    /// Creates a new image animation from the given parent
    ImageAnimation(const Resource::Animation &parent, bool create=false);

    /// Deletes this animation object
    virtual void DeleteAnimation() override {
        delete this;
    }

    virtual bool Progress(unsigned &leftover) override;

    virtual GL::Texture GetID() const {
        if(!current) return 0;

        return current->GetID();
    }

    virtual Geometry::Size GetImageSize() const {
        if(!current) return {0, 0};

        return current->GetImageSize();
    }

    virtual const Geometry::Pointf * GetCoordinates() const {
        if(!current) return Graphics::TextureSource::fullcoordinates;

        return current->GetCoordinates();
    }

protected:

    /// Parent of this animation
    const Resource::Animation &parent;

private:
    Image *current = nullptr;
};


class AnimationFrame {
public:
    AnimationFrame(Image &image, unsigned duration=42, unsigned start=0) : Duration(duration), Start(start), Visual(image) {}

    unsigned Duration;
    unsigned Start;
    Image &Visual;
};


/// This class represents an animation resource. Image animations can be created using this object. An animation object can be moved.
/// Duplicate function should be used to copy an animation.
class Animation : public Base, public virtual Graphics::RectangularAnimationProvider {
public:
    /// Default constructor
    Animation() : Base() { }

    /// Move constructor
    Animation(Animation &&other);

    /// Copy constructor is disabled, use Duplicate
    Animation(const Animation&) = delete;

    /// Move assignment
    Animation &operator =(Animation &&other);

    /// Copy assignment is disabled, use Duplicate
    Animation &operator =(const Animation &other) = delete;

    /// Swaps two animation, used for move semantics
    void Swap(Animation &other);

    /// Duplicates this resource
    void Duplicate() const;

    /// Returns the Gorgon Identifier
    virtual GID::Type GetGID() const override {
        return GID::Animation;
    }

    /// Returns the size of the first image
    Geometry::Size GetSize() const {
        if(frames.size()>0)
            return frames[0].Visual.GetSize();
        return {0, 0};
    }

    /// Returns number of frames
    int GetCount() const {
        return frames.size();
    }

    /// Creates a new animation from this resource
    virtual const ImageAnimation &CreateAnimation(Gorgon::Animation::Timer &controller, bool owner=false) const override {
        return *new ImageAnimation(*this, controller, owner);
    }

    /// Creates a new animation from this resource
    virtual const ImageAnimation &CreateAnimation(bool create=false) const override {
        return *new ImageAnimation(*this, create);
    }

    /// Returns the image that is to be shown at the given time. If the given time is larger
    /// than the animation duration, animation is assumed to be looping.
    Image &ImageAt(unsigned time) const {
#ifndef NDEBUG
        if(GetDuration()==0) {
            throw std::runtime_error("Animation is empty");
        }
#endif
        time=time%GetDuration();

        return frames[FrameAt(time)].Visual;
    }

    /// Returns the image at the given frame
    Image &operator [](int frame) const {
        return frames[frame].Visual;
    }

    /// Returns which frame is at the given time. If the given time is larger than the animation
    /// duration, last frame is returned.
    unsigned FrameAt(unsigned time) const;

    /// Returns the starting time of the given frame
    unsigned StartOf(unsigned frame) const {
        return frames[frame].Start;
    }

    /// Returns the duration of the animation
    unsigned GetDuration() const {
        return duration;
    }

    /// Returns the duration of the given frame
    unsigned GetDuration(unsigned frame) const {
        return frames[frame].Duration;
    }

    /// This function allows loading animation with a function to load unknown resources. The supplied function should
    /// call LoadObject function of File class if the given GID is unknown.
    static Animation *LoadResourceWith(File &file, std::istream &data, unsigned long size,
                                       std::function<Base*(File &, std::istream&, GID::Type, unsigned long)> loadfn);

    /// This function loads an animation resource from the given file
    static Animation *LoadResource(File &file, std::istream &data, unsigned long size) {
        return LoadResourceWith(file, data, size, {});
    }

protected:
    /// Frame durations
    std::vector<AnimationFrame> frames;

    /// Total duration
    unsigned duration;

};
}
}

#pragma warning(pop)
