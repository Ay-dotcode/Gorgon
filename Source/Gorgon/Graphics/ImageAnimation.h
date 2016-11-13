#pragma once

#include "Animations.h"
#include "Texture.h"
#include "Bitmap.h"

namespace Gorgon { namespace Graphics {
    
    template<class T_, template<class, class, class> class A_, class F_>
    class basic_TextureAnimationProvider;
    
    template<class T_>
    class basic_AnimationFrame {
    public:
        basic_AnimationFrame(T_ &image, unsigned duration = 42, unsigned start = 0) :
        image(&image), duration(duration), start(start) { }
        
        /// Returns the duration of this frame
        unsigned GetDuration() const {
            return duration;
        }
        
        /// Returns the starting time of this frame
        unsigned GetStart() const {
            return start;
        }
        
        /// Returns the ending time of this frame
        unsigned GetEnd() const {
            return start + duration;
        }
        
        /// Returns if the given time is within this frame
        bool IsIn(unsigned time) const {
            return time >= start && time < start + duration;
        }
        
        /// Returns the image for this frame
        T_ &GetImage() const {
            return *image;
        }
        
    private:
        T_ *image;
        unsigned duration, start;
    };
    
    template<class T_, class P_, class F_>
    class basic_TextureAnimation : public virtual Image, public virtual RectangularAnimation {
    public:
        using ParentType = P_;
        using FrameType  = F_;
        
		/// Creates a new image animation from the given parent
		basic_TextureAnimation(const ParentType &parent, Gorgon::Animation::Timer &controller) :
		parent(&parent), Gorgon::Animation::Base(controller) { }

		/// Creates a new image animation from the given parent
        basic_TextureAnimation(const ParentType &parent, bool create) :
		parent(&parent), Gorgon::Animation::Base(create) { }

		/// Deletes this animation object
		virtual void DeleteAnimation() override {
			delete this;
		}
        
		virtual bool Progress(unsigned &leftover) override;
        
        virtual GL::Texture GetID() const override {
            if(current)
                return current->GetImage().GetID();
            else if(parent->GetCount()) 
                return (*parent)[0].GetID();

            Utils::ASSERT_FALSE("Animation contains no frames.");
            
            return 0;
        }
        
        virtual Geometry::Size GetImageSize() const override {
            if(current)
                return current->GetImage().GetImageSize();
            else if(parent->GetCount()) 
                return (*parent)[0].GetImageSize();

            Utils::ASSERT_FALSE("Animation contains no frames.");
            
            return {0, 0};
        }
        
        virtual ColorMode GetMode() const override {
            if(current)
                return current->GetImage().GetMode();
            else if(parent->GetCount()) 
                return (*parent)[0].GetMode();

            Utils::ASSERT_FALSE("Animation contains no frames.");
            
            return ColorMode::RGBA;
        }
        
        virtual const Geometry::Pointf *GetCoordinates() const override {
            if(current)
                return current->GetImage().GetCoordinates();
            else if(parent->GetCount()) 
                return (*parent)[0].GetCoordinates();

            Utils::ASSERT_FALSE("Animation contains no frames.");
            
            return nullptr;
        }
        
    private:
        const ParentType *parent = nullptr;
        const FrameType  *current = nullptr;
    };
    
    template<class T_, template<class, class, class> class A_, class F_>
    class basic_TextureAnimationProvider : public RectangularAnimationProvider {
    public:
        using Iterator = typename std::vector<basic_AnimationFrame<T_>>::iterator;
        using ConstIterator = typename std::vector<basic_AnimationFrame<T_>>::const_iterator;
        using AnimationType = basic_TextureAnimation<T_, basic_TextureAnimationProvider<T_, A_, F_>, F_>;
        using FrameType = F_;

		/// Returns the size of the first image
		Geometry::Size GetSize() const {
			if(frames.size()>0)
				return frames[0].GetImage().GetSize();
            
			return {0, 0};
		}

		/// Returns number of frames
		int GetCount() const {
			return frames.size();
		}

		/// Creates a new animation from this resource
		virtual const AnimationType &CreateAnimation(Gorgon::Animation::Timer &controller) const override {
			return *new AnimationType(*this, controller);
		}

		/// Creates a new animation from this resource
		virtual const AnimationType &CreateAnimation(bool create=true) const override {
			return *new AnimationType(*this, create);
		}

		/// Returns the image that is to be shown at the given time. If the given time is larger
		/// than the animation duration, animation is assumed to be looping.
		T_ &ImageAt(unsigned time) const {
            ASSERT(frames.size() || duration==0, "Animation is empty");
            
			time=time%GetDuration();

			return frames[FrameAt(time)].GetImage();
		}

		/// Returns the image at the given frame
		T_ &operator [](int frame) const {
            ASSERT(frame>=0 && frame<(int)frames.size(), "Index out of bounds");
            
			return frames[frame].GetImage();
		}

		/// Returns which frame is at the given time. If the given time is larger than the animation
		/// duration, last frame is returned.
		unsigned FrameIndexAt(unsigned time) const {
            ASSERT(frames.size(), "Animation is empty");
            
            int i = 0;
            for(auto f : frames) {
                if(f.GetEnd() >= time)
                    return i;
                
                i++;
            }
            
            return i-1;
        }
        
        /// Returns the frame at specific point
        const F_ &FrameAt(int index) const {
            ASSERT(index>=0 && index<(int)frames.size(), "Index out of bounds");
            
            return frames[index];
        }

		/// Returns the starting time of the given frame
		unsigned StartOf(unsigned frame) const {
			return frames[frame].GetStart();
		}

		/// Returns the duration of the animation
		unsigned GetDuration() const {
			return duration;
		}

		/// Returns the duration of the given frame
		unsigned GetDuration(unsigned frame) const {
            ASSERT(frame>=0 && frame<frames.size(), "Index out of bounds");
            
			return frames[frame].GetDuration();
		}
		
		/// Adds the given image to the end of the animation
		void Add(Image &image, unsigned duration = 42) {
			frames.push_back({image, duration, this->duration});
			this->duration += duration;
		}
		
		/// Inserts the given image before the given frame
		void Insert(Image &image, unsigned before, unsigned duration = 42) {
            ASSERT(before < frames.size(), "Index out of bounds");
            
			frames.insert(frames.begin()+before, {image, duration, frames[before].GetStart()});
			
			for(unsigned i=before+1; i<frames.size(); i++) {
				frames[i].start=frames[i-1].GetEnd();
			}
			
			this->duration += duration;
		}
		
		/// Removes an image from the animation
		void Remove(int frame) {
            ASSERT(frame>=0 && frame < frames.size(), "Index out of bounds");
            
            frames.remove(frames.begin() + frame);
        }
        
        /// Removes an image from the animation
        void Remove(ConstIterator it) {
            frames(it);
        }
		
		/// Returns an iterator to the beginning of the animation frames
		Iterator begin() {
			return frames.begin();
		}
		
		/// Returns an iterator to the beginning of the animation frames
		ConstIterator begin() const {
			return frames.begin();
		}
		
		/// Returns an iterator to the end of the animation frames
		Iterator end() {
			return frames.end();
		}
		
		/// Returns an iterator to the end of the animation frames
		ConstIterator end() const {
			return frames.end();
		}
        
    private:
        std::vector<basic_AnimationFrame<T_>> frames;
        unsigned duration = 0;
    };
    
    template<class T_, class P_, class F_>
    bool basic_TextureAnimation<T_, P_, F_>::Progress(unsigned &leftover) {
		if(!controller) return false;

		if(parent->GetCount()==0) return false;
        

		unsigned progress=controller->GetProgress();


        if(controller->IsControlled()) {
            if(progress>parent->GetDuration()) {
                current=&parent->FrameAt(parent->GetCount()-1);
                leftover=progress-parent->GetDuration();
                
                return false;
            }
            else {
                current=&parent->FrameAt(parent->FrameIndexAt(progress));
                
                return true;
            }
        }
        else {
            current = &parent->FrameAt(parent->FrameIndexAt(progress % parent->GetDuration()));
            
            return true;
        }
    }
    
    using BitmapAnimationProvider = basic_TextureAnimationProvider<Bitmap, basic_TextureAnimation, basic_AnimationFrame<Bitmap>>;
    
    using ImageAnimationProvider = basic_TextureAnimationProvider<Image, basic_TextureAnimation, basic_AnimationFrame<Image>>;
    
} }
