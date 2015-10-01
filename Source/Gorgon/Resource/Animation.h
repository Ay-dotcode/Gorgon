#pragma once

#include "Base.h"
#include "../Animation.h"
#include "Image.h"

#pragma warning(push)
#pragma warning(disable:4250)

namespace Gorgon { namespace Resource {
	class File;
	class Reader;

	class Animation;

	/// This class is a created image animation that can be controlled and drawn over screen.
	class ImageAnimation :
		public virtual Graphics::RectangularAnimation, public virtual Graphics::Image, public virtual Graphics::TextureSource
	{
	public:
		/// Creates a new image animation from the given parent
			ImageAnimation(const Resource::Animation &parent, Gorgon::Animation::Timer &controller);

		/// Creates a new image animation from the given parent
			ImageAnimation(const Resource::Animation &parent, bool create);

		/// Deletes this animation object
		virtual void DeleteAnimation() override {
			delete this;
		}

		virtual bool Progress(unsigned &leftover) override;

		virtual GL::Texture GetID() const {
			if(!current) return 0;

			return current->GetID();
		}

			virtual Geometry::Size GetImageSize() const override {
			if(!current) return {0, 0};

			return current->GetImageSize();
		}

			virtual const Geometry::Pointf *GetCoordinates() const override {
			if(!current) return Graphics::TextureSource::fullcoordinates;

			return current->GetCoordinates();
		}

	protected:

		/// Parent of this animation
		const Resource::Animation &parent;

	private:
		Image *current = nullptr;
	};


	/// This class represents a single frame in a resource animation
	class AnimationFrame {
		friend class Animation;
	public:
		/// Default constructor requires image. Default duration is set to 42 denoting 24fps 
		AnimationFrame(Image &image, unsigned duration=42, unsigned start=0) : duration(duration), start(start), visual(&image) {}

		/// Returns the duration of this frame
		unsigned GetDuration() const {
			return duration;
		}

		/// Returns the starting time of this frame
		unsigned GetStart() const {
			return start;
		}

		/// Returns the image of this frame
		Image &GetImage() const {
			return *visual;
		}

		/// Returns the time that ending this frame will be changed. The returned time is out of the duration of this frame
		unsigned GetEnd() const {
			return start+duration;
		}

		/// Returns whether the given time is within the duration of this frame
		bool IsIn(unsigned time) const  {
			return time>=start && time-start<duration;
		}

	private:
		Image *visual;
		unsigned duration, start;
	};


	/// This class represents an animation resource. Image animations can be created using this object. An animation object can be moved.
	/// Duplicate function should be used to copy an animation.
	class Animation : public Base, public virtual Graphics::RectangularAnimationProvider {
	public:
		/// Default constructor
		Animation() : Base() { }

		/// Move constructor
		Animation(Animation &&other);

		/// Copy constructor is disabled, use Duplicate or DeepDuplicate
		Animation(const Animation&) = delete;

		/// Move assignment
		Animation &operator =(Animation &&other);

		/// Copy assignment is disabled, use Duplicate
		Animation &operator =(const Animation &other) = delete;

		/// Swaps two animation, used for move semantics
		void Swap(Animation &other);

		/// Duplicates this resource. Does not duplicate resource related data.
		Animation Duplicate() const {
			Animation ret;
			ret.frames = frames;
			ret.duration = duration;

			return ret;
		}

		/// Returns the Gorgon Identifier
		virtual GID::Type GetGID() const override {
			return GID::Animation;
		}

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
		virtual const ImageAnimation &CreateAnimation(Gorgon::Animation::Timer &controller) const override {
			return *new ImageAnimation(*this, controller);
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

			return frames[FrameAt(time)].GetImage();
		}

		/// Returns the image at the given frame
		Image &operator [](int frame) const {
			return frames[frame].GetImage();
		}

		/// Returns which frame is at the given time. If the given time is larger than the animation
		/// duration, last frame is returned.
		unsigned FrameAt(unsigned time) const;

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
			return frames[frame].GetDuration();
		}
		
		/// Adds the given image to the end of the animation
		void Add(Image &image, unsigned duration = 42) {
			frames.push_back({image, duration, this->duration});
			this->duration += duration;
		}
		
		/// Inserts the given image before the given frame
		void Insert(Image &image, unsigned before, unsigned duration = 42) {
#ifndef NDEBUG
			if(before >= frames.size()) {
				throw std::runtime_error("Index that is tried to insert before, do not exists.");
			}
#endif
			frames.insert(frames.begin()+before, {image, duration, frames[before].GetStart()});
			
			for(unsigned i=before+1; i<frames.size(); i++) {
				frames[i].start=frames[i-1].GetEnd();
			}
			
			this->duration += duration;
		}
		
		/// Returns an iterator to the beginning of the animation frames
		std::vector<AnimationFrame>::iterator begin() {
			return frames.begin();
		}
		
		/// Returns an iterator to the beginning of the animation frames
		std::vector<AnimationFrame>::const_iterator begin() const {
			return frames.begin();
		}
		
		/// Returns an iterator to the end of the animation frames
		std::vector<AnimationFrame>::iterator end() {
			return frames.end();
		}
		
		/// Returns an iterator to the end of the animation frames
		std::vector<AnimationFrame>::const_iterator end() const {
			return frames.end();
		}
		
		/// This function allows loading animation with a function to load unknown resources. The supplied function should
		/// call LoadObject function of File class if the given GID is unknown.
		static Animation *LoadResourceWith(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long size,
										std::function<Base*(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, GID::Type, unsigned long)> loadfn);

		/// This function loads an animation resource from the given file
		static Animation *LoadResource(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long size) {
			return LoadResourceWith(file, reader, size, {});
		}

	protected:
		
		void save(Writer &writer) override { }

		/// Frame durations
		std::vector<AnimationFrame> frames;

		/// Total duration
		unsigned duration = 0;

	};
} }

#pragma warning(pop)
