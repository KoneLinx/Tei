#pragma once

#include <ranges>

namespace tei::internal::utility
{

	template <std::ranges::random_access_range Rng>
	constexpr auto RangePerIndex(Rng& rng) noexcept
	{
		namespace r = std::ranges;
		namespace v = std::views;

		using Size = r::range_size_t<Rng>;

		auto const valid = [&rng](Size index) -> bool
		{
			return index < size(rng);
		};
		auto const get = [&rng](Size index) -> r::range_reference_t<Rng>
		{
			return rng[index];
		};

		return v::iota(Size()) | v::take_while(valid) | v::transform(get);
	}

	template <typename It, typename Sentinel>
	auto SubrangeFromPair(std::pair<It, Sentinel> range)
	{
		return std::ranges::subrange{ range.first, range.second };
	}

	template <typename ViewIterator>
	decltype(auto) IteratorBase(ViewIterator&& it) 
	{
		if constexpr (requires { it.base(); })
			return IteratorBase(it.base());
		else
			return it;
	}

}
