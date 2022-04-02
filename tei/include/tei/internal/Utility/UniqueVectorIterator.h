#pragma once

#include <vector>
#include <memory>
#include <cassert>
#include <type_traits>

namespace tei::internal::utility
{

	template <typename ValueType>
	class UniqueVectorView
	{

		using Container = std::conditional_t<
			std::is_const_v<ValueType>,
			std::vector<std::unique_ptr<std::remove_const_t<ValueType>>> const,
			std::vector<std::unique_ptr<std::remove_const_t<ValueType>>>
		>;

	public:

		constexpr UniqueVectorView(Container& container)
			: m_Data{ std::addressof(container) }
		{}

		class Iterator : public std::forward_iterator_tag
		{

			Iterator(Container* cont, size_t index = {})
				: m_Data{ cont }
				, m_Index{ index }
			{}

			friend UniqueVectorView;

		public:
			
			Iterator()
				: Iterator{ nullptr }
			{}

			using difference_type = std::make_signed_t<size_t>;
			using value_type = ValueType;
			using pointer = std::add_pointer_t<ValueType>;
			using reference = std::add_lvalue_reference_t<ValueType>;

			reference operator * () const
			{
				return *operator->();
			}

			pointer operator -> () const
			{
				return std::to_address(m_Data->at(m_Index));
			}

			Iterator& operator ++ ()
			{
				++m_Index;
				return *this;
			}
			
			Iterator operator ++ (int)
			{
				return { m_Data, m_Index++ };
			}

			bool operator == (Iterator const& other) const
			{
				if (other.m_Data == nullptr)
				{
					return m_Data == nullptr || m_Index == m_Data->size();
				}
				else 
				if (m_Data == nullptr)
				{
					return other.m_Index == other.m_Data->size();
				}
				else
				{
					assert(m_Data == other.m_Data);
					return m_Index == other.m_Index;
				}
			}

		private:

			Container* m_Data;
			size_t m_Index;

		};

		Iterator begin()
		{
			return { m_Data };
		}

		Iterator end()
		{
			return {};
		}

	private:

		Container* m_Data;

	};

}