#pragma once

#include <optional>
#include <concepts>
#include <functional>

namespace tei::internal::utility
{

	template <typename To, typename From>
	inline auto& RefAs(From& var) noexcept
	{
		using P = std::conditional_t<std::is_pointer_v<From>, To*, To>;
		using R = std::conditional_t<std::is_const_v<From>, P const&, P &>;
		return reinterpret_cast<R>(var);
	}
	
	template <bool deference = false, bool as_const = false>
	struct wrapped_ptr_projector;

	template <>
	struct wrapped_ptr_projector<false, false>
	{	
		constexpr auto* operator () (auto const& uptr) const noexcept
		{
			return std::to_address(uptr);
		}
	};

	template <>
	struct wrapped_ptr_projector<false, true> : wrapped_ptr_projector<>
	{	
		constexpr auto const* operator () (auto const& uptr) const noexcept
		{
			return wrapped_ptr_projector<>::operator()(uptr);
		}
	};

	template <bool as_const>
	struct wrapped_ptr_projector<true, as_const> : wrapped_ptr_projector<false, as_const>
	{	
		constexpr auto& operator () (auto const& uptr) const noexcept
		{
			return *wrapped_ptr_projector<false, as_const>::operator()(uptr);
		}
	};

	struct as_const_projector
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

	template <size_t index>
	struct tuple_index_projector
	{
		template <typename T>
		constexpr auto operator () (T&& tup) -> decltype(auto)
		{
			return std::get<index>(std::forward<T>(tup));
		}
	};

	template <typename projector = void, typename ... rest>
	struct projector_sequence : projector, projector_sequence<rest...>
	{
		template <typename T>
		auto operator () (T&& obj) -> decltype(auto)
		{
			return projector_sequence<rest...>::operator()(projector::operator()(std::forward<T>(obj)));
		}
	};

	template <>
	struct projector_sequence<void> : std::identity
	{};

}