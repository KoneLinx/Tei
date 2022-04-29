#pragma once

#include <type_traits>

namespace tei::internal::utility
{


	template <typename To, typename From>
	inline auto& RefAs(From& var) noexcept
	{
		using P = std::conditional_t<std::is_pointer_v<From>, To*, To>;
		using R = std::conditional_t<std::is_const_v<From>, P const&, P&>;
		return reinterpret_cast<R>(var);
	}

	template <template <typename> typename Trait, template <typename ...> typename Class, typename ... Types>
	Class<Trait<Types>...> ApplyTraits_fn(Class<Types...>);

	template <template <typename> typename Trait, typename Class>
	using ApplyTrait_t = decltype(ApplyTraits_fn<Trait>(std::declval<Class>()));

}