#pragma once

#include "Base.h"
#include "../Animation.h"
#include "Image.h"
#include "../Graphics/ImageAnimation.h"

#pragma warning(push)
#pragma warning(disable:4250)

namespace Gorgon { namespace Resource {
	class File;
	class Reader;

	class Animation;


	/// This class represents an animation resource. Image animations can be created using this object. An animation object can be moved.
	/// Duplicate function should be used to copy an animation.
	class Animation : public Base, public Graphics::BitmapAnimationProvider {
	public:
		/// Default constructor
		Animation() : Base() {}

		/// Conversion constructor
		Animation(Graphics::BitmapAnimationProvider &&anim) : Base(), Graphics::BitmapAnimationProvider(std::move(anim)) {
		}

		/// Copy constructor is disabled, use Duplicate or DeepDuplicate
		Animation(const Animation&) = delete;

		/// Copy assignment is disabled, use Duplicate
		Animation &operator =(const Animation &other) = delete;

		/// Returns the Gorgon Identifier
		virtual GID::Type GetGID() const override {
			return GID::Animation;
		}
		
		/// Moves the animation out of the resource system.
		Graphics::BitmapAnimationProvider MoveOut();
		
		/// This function allows loading animation with a function to load unknown resources. The supplied function should
		/// call LoadObject function of File class if the given GID is unknown.
		static Animation *LoadResourceWith(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long size,
										std::function<Base*(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, GID::Type, unsigned long)> loadfn);

		/// This function loads an animation resource from the given file
		static Animation *LoadResource(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long size) {
			return LoadResourceWith(file, reader, size, {});
		}
		
		/// Saves the given animation as a resource. If the given animation is already a resource, its own save function
        /// will be used. Extra function can be used to save extra data related with this resource.
		static void SaveThis(Writer &writer, const Graphics::BitmapAnimationProvider &anim, GID::Type type = GID::Animation, std::function<void(Writer &writer)> extra = {});

	protected:
        virtual ~Animation() { }
		
		void save(Writer &writer) const override;

		// Two part save system allows objects that are derived from animation to exist.
		void savedata(Writer &writer) const;
	};
} }

#pragma warning(pop)
