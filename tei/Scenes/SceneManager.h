#pragma once

#include <vector>
#include <memory>

#include <tei/internal/ecs.h>
#include <tei/internal/utility.h>

namespace tei::internal::scene
{
	extern utility::Service<ecs::Object> Scene;
}

namespace tei::external
{
	constexpr static auto const& Scene{ tei::internal::scene::Scene };
}
