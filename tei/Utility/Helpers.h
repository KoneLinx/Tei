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
	
	template <bool as_const = false>
	struct wrapped_ptr_projector;

	template <>
	struct wrapped_ptr_projector<false>
	{	
		auto* operator () (auto const& uptr) const noexcept
		{
			return std::to_address(uptr);
		}
	};

	template <>
	struct wrapped_ptr_projector<true> : wrapped_ptr_projector<false>
	{	
		auto const* operator () (auto const& uptr) const noexcept
		{
			return wrapped_ptr_projector<>::operator()(uptr);
		}
	};

	struct as_const_projector
	{
		template <typename T>
		auto operator () (T& r) -> decltype(auto)
		{
			return std::as_const(r);
		}

		// Exception for pointers
		template <typename T> requires std::is_pointer_v<T>
		std::remove_pointer_t<T> const* operator () (T&& p)
		{
			return p;
		}

		// Exception for rvalues and rvalue references, (but not for rvalue pointers)
		template <typename T> requires (!std::is_lvalue_reference_v<T> && !std::is_pointer_v<T>)
		auto operator () (T&& rv) -> decltype(auto)
		{
			return static_cast<T const&&>(rv);
		}

		// See .cpp for tests
	};

	template <size_t index>
	struct tuple_index_projector
	{
		template <typename T>
		auto operator () (T&& tup) -> decltype(auto)
		{
			return std::get<index>(std::forward<T>(tup));
		}
	};

}