#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#include <typeinfo>
#endif // DEBUG

#include <memory>

namespace tei::internal::utility
{

	class AnyReference
	{
	public:

		inline AnyReference(std::nullptr_t) noexcept
			: m_Data{ nullptr }
#if defined(DEBUG) || defined(_DEBUG)
			, m_Type{ &typeid(std::nullptr_t) }
#endif
		{}

		inline AnyReference(auto const& value) noexcept
			: m_Data{ std::addressof(value) }
#if defined(DEBUG) || defined(_DEBUG)
			, m_Type{ &typeid(value) }
#endif
		{}

		template <typename T>
		inline operator T const& () const
		{
			return cast<T>();
		}

		inline bool operator == (AnyReference const& other) const
		{
			return m_Data == other.m_Data;
		}

		inline bool operator == (std::nullptr_t) const
		{
			return m_Data == nullptr;
		}

	protected:

		template <typename T>
		inline T const& cast() const
		{
#if defined(DEBUG) || defined(_DEBUG)
			assert(typeid(T) == *m_Type);
#endif
			return *static_cast<T const*>(m_Data);
		}

		void const* m_Data{};

#if defined(DEBUG) || defined(_DEBUG)
		std::type_info const* m_Type{};
#endif 

		friend std::pointer_traits<AnyReference>;

	};

	class AnyMutable
	{
	public:
		
		inline AnyMutable(std::nullptr_t) noexcept
			: m_Data{ nullptr }
#if defined(DEBUG) || defined(_DEBUG)
			, m_Type{ &typeid(std::nullptr_t) }
#endif
		{}

		inline AnyMutable(auto& value) noexcept
			: m_Data{ static_cast<void*>(std::addressof(value)) }
#if defined(DEBUG) || defined(_DEBUG)
			, m_Type{ &typeid(value) }
#endif
		{}

		template <typename T>
		inline operator T& () const noexcept
		{
			return cast<T>();
		}

		inline auto operator <=> (AnyMutable other) const
		{
			return m_Data <=> other.m_Data;
		}

		inline bool operator == (std::nullptr_t) const
		{
			return m_Data == nullptr;
		}

	protected:

		template <typename T>
		inline T& cast() const
		{
#if defined(DEBUG) || defined(_DEBUG)
			assert(typeid(T) == *m_Type);
#endif
			return *static_cast<T*>(m_Data);
		}

		void* m_Data;

#if defined(DEBUG) || defined(_DEBUG)
		std::type_info const* m_Type;
#endif 

		friend std::pointer_traits<AnyMutable>;

	};

}

template <>
struct std::pointer_traits<tei::internal::utility::AnyReference>
{
	static void const* to_address(tei::internal::utility::AnyReference const& anyref) noexcept
	{
		return anyref.m_Data;
	}
};

template <>
struct std::hash<tei::internal::utility::AnyReference>
{
	inline auto operator () (tei::internal::utility::AnyReference const& anyref) const
	{
		return std::hash<void const*>{}(std::to_address(anyref));
	}
};

template <>
struct std::pointer_traits<tei::internal::utility::AnyMutable>
{
	static void* to_address(tei::internal::utility::AnyMutable const& anyref) noexcept
	{
		return anyref.m_Data;
	}
};

template <>
struct std::hash<tei::internal::utility::AnyMutable>
{
	inline auto operator () (tei::internal::utility::AnyMutable const& anyref) const
	{
		return std::hash<void const*>{}(std::to_address(anyref));
	}
};
