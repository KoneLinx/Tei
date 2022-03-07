
#include "SceneManager.h"

#include <ranges>
#include <algorithm>

#include <tei/internal/utility.h>

namespace tei::internal::scene
{
	utility::Service<ecs::Object> Scene{};
}
