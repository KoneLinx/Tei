#pragma once

#include <type_traits>
#include <utility>

namespace tei::internal::utility::projectors
{

	struct addressof
	{
		constexpr auto operator () (auto& tup) -> decltype(auto)
		{
			return std::addressof(tup);
		}
	};

}
