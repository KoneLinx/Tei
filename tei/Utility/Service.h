#pragma once

#include <type_traits>
#include <memory>

namespace tei::internal::utility
{

	struct Registerer
	{
		void operator () (auto*, auto*) {}
	};

	template <typename Served, typename Registerer = Registerer>
	class Service final
	{
		using Mutable = std::remove_const_t<Served>;

	public:

		Served* operator -> () const;
		operator Served& () const;
		operator bool() const;

		Mutable* operator -> ();
		Mutable& operator *  ();
		operator Mutable& ();

		Service() = default;
		~Service();

		Mutable& Register(Mutable* service);

	private:

		std::unique_ptr<Mutable> m_Service;

	};

}

#pragma region _impl

namespace tei::internal::utility
{

	template<typename Served, typename Registerer>
	inline Served* Service<Served, Registerer>::operator->() const
	{
		return m_Service.get();
	}

	template<typename Served, typename Registerer>
	inline Service<Served, Registerer>::operator Served& () const
	{
		return *m_Service;
	}

	template<typename Served, typename Registerer>
	inline Service<Served, Registerer>::Mutable* Service<Served, Registerer>::operator->()
	{
		return m_Service.get();
	}

	template<typename Served, typename Registerer>
	inline Service<Served, Registerer>::Mutable& Service<Served, Registerer>::operator*()
	{
		return *m_Service;
	}

	template<typename Served, typename Registerer>
	inline Service<Served, Registerer>::operator Mutable& ()
	{
		return *m_Service;
	}

	template<typename Served, typename Registerer>
	inline Service<Served, Registerer>::operator bool() const
	{
		return bool(m_Service);
	}

	template<typename Served, typename Registerer>
	inline Service<Served, Registerer>::~Service()
	{
		Register(nullptr);
	}

	template<typename Served, typename Registerer>
	inline Service<Served, Registerer>::Mutable& Service<Served, Registerer>::Register(Mutable* service)
	{
		Registerer{}(std::to_address(m_Service), service);
		m_Service.reset(service);
		return *service;
	}

}

#pragma endregion