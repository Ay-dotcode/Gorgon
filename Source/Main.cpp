#include "Main.h"

#include "Filesystem.h"
#include "WindowManager.h"

namespace Gorgon {

	namespace internal {
		std::string systemname;
	}

	void Initialize(const std::string &name) {
		internal::systemname=name;

		Filesystem::Initialize();
		WindowManager::Initialize();
	}
}
