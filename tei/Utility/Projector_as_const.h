#pragma once

#include <type_traits>
#include <utility>

namespace tei::internal::utility::projectors
{

	struct as_const
	{
		template <typename T>
		constexpr auto operator () (T& r) -> decltype(auto)
		{
			return std::as_const(r);
		}

		// Exception for pointers
		template <typename T> requires std::is_pointer_v<T>
		constexpr std::remove_pointer_t<T> const* operator () (T&& p)
		{
			return p;
		}

		// Exception for rvalues and rvalue references, (but not for rvalue pointers)
		template <typename T> requires (!std::is_lvalue_reference_v<T> && !std::is_pointer_v<T>)
		constexpr auto operator () (T&& rv) -> decltype(auto)
		{
			return static_cast<T const&&>(rv);
		}

		// See .cpp for tests
	};

}
