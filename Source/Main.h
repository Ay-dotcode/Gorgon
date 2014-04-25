#pragma once

#include <string>

/// Root namespace for Gorgon Game Engine.
namespace Gorgon {

	/// @cond INTERNAL
	namespace internal {
		extern std::string systemname;
	}
	/// @endcond

	/// Initializes the entire system
	void Initialize(const std::string &systemname);

	/// Returns the name of the current system
	inline std::string GetSystemName() { return internal::systemname; }
}
