#pragma once

#include <type_traits>
#include <utility>

namespace tei::internal::utility
{

	template <typename Data>
	class Observed
	{
	public:

		constexpr Observed(bool initial = true) requires std::default_initializable<Data>
			: Observed(Data{}, initial)
		{}

		constexpr Observed(Data data, bool initial = true) requires std::movable<Data>
			: m_Data{ std::move(data) }
			, m_Updated{ initial }
		{}

		constexpr Observed& operator = (Data const& data) requires std::copyable<Data>
		{
			if constexpr (std::equality_comparable<Data>)
			{
				if (m_Data == data)
					return *this;
			}
			m_Updated = true;
			m_Data = data;
			return *this;
		}
		constexpr Observed& operator = (Data && data) requires std::movable<Data>
		{
			if constexpr (std::equality_comparable<Data>)
			{
				if (m_Data == data)
					return *this;
			}
			m_Updated = true;
			m_Data = std::move(data);
			return *this;
		}

		constexpr Data const& operator * () const noexcept
		{
			return m_Data;
		}

		constexpr Data const* operator -> () const noexcept
		{
			return &m_Data;
		}

		constexpr operator Data const& () const noexcept
		{
			return m_Data;
		}

		constexpr explicit operator Data& () noexcept
		{
			m_Updated = true;
			return m_Data;
		}

		template <std::three_way_comparable_with<Data> T>
		constexpr auto operator <=> (T const& other) const
		{
			return m_Data <=> other;
		}

		template <std::equality_comparable_with<Data> T>
		constexpr bool operator == (T const& other) const
		{
			return m_Data == other;
		}

		constexpr Data& get() noexcept
		{
			m_Updated = true;
			return m_Data;
		}

		constexpr bool check() const noexcept
		{
			return m_Updated;
		}
		constexpr bool checkout() noexcept
		{
			return std::exchange(m_Updated, false);
		}

	private:
		Data m_Data;
		bool m_Updated{ true };
	};

	template <>
	class Observed<void> : Observed<std::nullptr_t>
	{};

}
