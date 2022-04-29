#pragma once

#include <type_traits>
#include <utility>

namespace tei::internal::utility
{

	template <typename Object>
	class Static final
	{
		using Mutable = std::remove_const_t<Object>;

	public:

		template <typename ... Arg>
		explicit Static(Arg&& ... arg)
			: m_Object{ std::forward<Arg>(arg) ... }
		{}

		explicit Static() requires std::default_initializable<Object>
			: m_Object{}
		{}

		Object* operator -> () const;
		Object& operator *  () const;
		operator Object& () const;

		Mutable* operator -> ();
		Mutable& operator *  ();
		operator Mutable& ();

	private:

		Mutable m_Object;

	};

}

#pragma region _impl

namespace tei::internal::utility
{

	template<typename Object>
	inline Object* Static<Object>::operator->() const
	{
		return &m_Object;
	}

	template<typename Object>
	inline Object& Static<Object>::operator*() const
	{
		return m_Object;
	}

	template<typename Object>
	inline Static<Object>::operator Object& () const
	{
		return m_Object;
	}

	template<typename Object>
	inline Static<Object>::Mutable* Static<Object>::operator->()
	{
		return &m_Object;
	}

	template<typename Object>
	inline Static<Object>::Mutable& Static<Object>::operator*()
	{
		return m_Object;
	}

	template<typename Object>
	inline Static<Object>::operator Mutable& ()
	{
		return m_Object;
	}

}

#pragma endregion