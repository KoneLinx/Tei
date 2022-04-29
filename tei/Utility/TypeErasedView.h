#pragma once

#include "AnyReference.h"

#include <functional>
#include <ranges>
#include <optional>
#include <type_traits>
#include <any>

namespace tei::internal::utility
{

	template <typename ValueType>
	class ForwardView
	{

		using AnyReference = std::conditional_t<std::is_const_v<ValueType>, AnyReference, AnyMutable>;

	public:

		template <std::ranges::forward_range Rng> requires std::is_same_v<std::remove_const_t<std::ranges::range_value_t<Rng>>, std::remove_const_t<ValueType>>
		ForwardView(Rng);

		class Iterator : public std::forward_iterator_tag
		{

			Iterator(std::function<AnyReference()> next)
				: m_Value{ next() }
				, m_Next{ std::move(next) }
			{}

			friend ForwardView;

		public:
			
			Iterator()
				: m_Value{ nullptr }
				, m_Next{}
			{}

			using difference_type = std::make_signed_t<size_t>;
			using value_type = ValueType;
			using pointer = std::add_pointer_t<ValueType>;
			using reference = std::add_lvalue_reference_t<ValueType>;

			ValueType& operator * () const
			{
				return m_Value;
			}

			ValueType* operator -> () const
			{
				return &operator*();
			}

			Iterator& operator ++ ()
			{
				m_Value = m_Next();
				return *this;
			}
			
			Iterator operator ++ (int)
			{
				Iterator copy{ *this };
				operator++();
				return copy;
			}

			bool operator == (Iterator const& other) const
			{
				return std::to_address(m_Value) == std::to_address(other.m_Value);
			}

		private:

			AnyReference m_Value;
			std::function<AnyReference()> m_Next;

		};

		Iterator begin();
		Iterator end();

	private:

		std::any m_Data;
		std::function<AnyReference()> m_Begin;

	};

	template<typename ValueType>
	template<std::ranges::forward_range Rng> requires std::is_same_v<std::remove_const_t<std::ranges::range_value_t<Rng>>, std::remove_const_t<ValueType>>
	inline ForwardView<ValueType>::ForwardView(Rng rng)
		: m_Data{ std::move(rng) }
		, m_Begin{
			[sub = std::ranges::subrange{ std::any_cast<Rng&>(m_Data) }] () mutable -> AnyReference
			{
				if (sub.empty())
				{
					return nullptr;
				}
				else
				{
					auto it = sub.begin();
					sub.advance(1);
					return *it;
				}
			}
		}
	{}

	template<typename ValueType>
	inline ForwardView<ValueType>::Iterator ForwardView<ValueType>::begin()
	{
		return m_Begin;
	}

	template<typename ValueType>
	inline ForwardView<ValueType>::Iterator ForwardView<ValueType>::end()
	{
		return {};
	}

}