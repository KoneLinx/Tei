#pragma once

#include <memory>

namespace tei::internal::utility::projectors
{

	template <bool deference = false, bool as_const = false>
	struct to_address;

	template <>
	struct to_address<false, false>
	{	
		constexpr auto* operator () (auto const& uptr) const noexcept
		{
			return std::to_address(uptr);
		}
	};

	template <>
	struct to_address<false, true> : to_address<false, false>
	{	
		constexpr auto const* operator () (auto const& uptr) const noexcept
		{
			return to_address<>::operator()(uptr);
		}
	};

	template <bool as_const>
	struct to_address<true, as_const> : to_address<false, as_const>
	{	
		constexpr auto& operator () (auto const& uptr) const noexcept
		{
			return *to_address<false, as_const>::operator()(uptr);
		}
	};

}