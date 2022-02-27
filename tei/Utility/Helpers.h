#pragma once

#include <optional>
#include <concepts>
#include <functional>

namespace tei::internal::utility
{

	//template <size_t I>
	//constexpr static auto Getter = [] (auto& obj) -> auto&
	//{
	//	return std::get<I>(obj);
	//};

	template <typename To, typename From>
	inline auto& RefAs(From& var) noexcept
	{
		using P = std::conditional_t<std::is_pointer_v<From>, To*, To>;
		using R = std::conditional_t<std::is_const_v<From>, P const&, P &>;
		return reinterpret_cast<R>(var);
	}

	//template <typename T>
	//inline constexpr T* TmpData(T&& temp) noexcept
	//{
	//	return &temp;
	//}

}