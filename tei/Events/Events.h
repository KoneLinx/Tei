#pragma once

#include <tei/components.h>
#include <tei/utility.h>

namespace tei::internal::events
{

	extern utility::Service<components::Subject> Event;

}

namespace tei::external
{

	inline constexpr auto const& Event{ tei::internal::events::Event };

}