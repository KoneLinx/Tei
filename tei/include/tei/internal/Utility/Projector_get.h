#pragma once

#include <utility>
#include <tuple>

namespace tei::internal::utility::projectors
{

	namespace detail
	{
		template <typename Tup, typename Type>
		concept getable_type = requires (Tup tup) { std::get<Type>(tup); };
		
		template <typename Tup, size_t INDEX>
		concept getable_index = requires (Tup tup) { std::get<INDEX>(tup); };
	}

	template <typename Type>
	struct get_type
	{
		template <detail::getable_type<Type> T>
		constexpr auto operator () (T&& tup) -> decltype(auto)
		{
			return std::get<Type>(std::forward<T>(tup));
		}
	};

	template <size_t INDEX>
	struct get_index
	{
		template <detail::getable_index<INDEX> T>
		constexpr auto operator () (T&& tup) -> decltype(auto)
		{
			return std::get<INDEX>(std::forward<T>(tup));
		}
	};

}
