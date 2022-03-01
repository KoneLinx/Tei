
#include "SceneManager.h"

#include <ranges>
#include <algorithm>

#include <tei/utility.h>

namespace tei::internal::scene
{
	utility::Service<ecs::Object> Scene{};
}
