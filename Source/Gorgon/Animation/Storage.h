#pragma once

#include <stdexcept>

#include "../Animation.h"

namespace Gorgon { namespace Animation {

	/**
	* This class stores animations as a part of itself so that it can be moved around as
	* a value rather than a reference.
	*/
	template<class A_>
	class basic_Storage : public virtual Provider {
	public:

		/// Empty constructor
		basic_Storage() = default;

		/// Filling constructor
		basic_Storage(const A_ &anim, bool owner = false) : anim(&anim), isowned(owner) { }

		/// Copy constructor is disabled for ownership reasons
		basic_Storage(const basic_Storage &) = delete;

		/// Move constructor
		basic_Storage(basic_Storage &&other) : anim(other.anim), owner(other.owner) {
			other.owner = false;
			other.anim  = nullptr;
		}

		/// Copy assignment
		basic_Storage &operator =(const basic_Storage &) = delete;

		/// Move assignment
		basic_Storage &operator =(basic_Storage &&other) {
			RemoveAnimation();
			owner = other.owner;
			anim = other.anim;
			other.owner = false;
			other.anim = nullptr;
		}

		/// Check if this storage has an animation
		bool HasAnimation() const {
			return anim != nullptr;
		}

		/// Returns the animation stored in the object. If
		/// there is no animation provider stored, it will throw std::runtime_error
		const A_ &GetAnimation() const {
			if(anim)
				return *anim;
			else
				throw std::runtime_error("Storage contains no animation");
		}

		/// Sets the animation stored in this container
		void SetAnimation(const A_ &value, bool owner = false) {
			RemoveAnimation();

			anim = value;
			this->owner = owner;
		}

		/// Removes the animation stored in the container, if the container owns
		/// the animation, it will be destroyed. Use Release to release resource
		/// without destroying it
		void RemoveAnimation() {
			if(owner)
				delete anim;

			owner = false;
			anim = nullptr;
		}

		/// Removes the animation from the storage without destroying it.
		const A_ *Release() {
			auto temp = anim;
			
			owned = false;

			RemoveAnimation();

			return temp;
		}

		/// Whether the stored animation is owned by this container
		bool IsOwner() const {
			return isowned;
		}

		/// This function creates a new animation from the stored animation provider. If
		/// there is no animation provider stored, it will throw std::runtime_error
		virtual typename A_::AnimationType &CreateAnimation(Timer &timer) const override {
			if(anim)
				return anim->CreateAnimation(timer);
			else
				throw std::runtime_error("Storage contains no animation");
		}

		/// This function creates a new animation from the stored animation provider. If
		/// there is no animation provider stored, it will throw std::runtime_error
		virtual typename A_::AnimationType &CreateAnimation(bool create=true) const override {
			if(anim)
				return anim->CreateAnimation(create);
			else
				throw std::runtime_error("Storage contains no animation");
		}

	private:
		const A_ *anim = nullptr;
		bool isowned = false;
	};

	/// Moves one type of animation into another.
	template<class Target_, class Original_>
	basic_Storage<Target_> AnimationCast(basic_Storage<Original_> &&original) {
		basic_Storage<Target_> target;

		bool owned = original.IsOwner();
		Target_ &anim = dynamic_cast<Target_&>(original.Release());
		target.Set(anim, owned);

		return target;
	}

	/// Basic animation storage, can store all types of animation and can be moved around as a value.
	using Storage = basic_Storage<Provider>;

} }