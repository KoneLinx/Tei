
#include <concepts>
#include <memory>

namespace tei::internal::utility
{

	//template <typename Registerer, typename Served>
	//concept ServiceRegisterer = std::invocable<Registerer, std::remove_const_t<Served>*, std::remove_const_t<Served>*> && std::is_default_constructible_v<Registerer>;
	//ServiceRegisterer<Served>

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
		operator bool() const;

		Mutable* operator -> ();
		Mutable& operator *  ();

		Service() = default;
		~Service();

		Mutable& Register(Mutable* service);

	private:

		std::unique_ptr<Mutable> m_Service;

	};

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

		Mutable* operator -> ();
		Mutable& operator *  ();

	private:

		Mutable m_Object;

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
	inline Service<Served, Registerer>::operator bool() const
	{
		return m_Service;
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
	inline Static<Object>::Mutable* Static<Object>::operator->()
	{
		return &m_Object;
	}

	template<typename Object>
	inline Static<Object>::Mutable& Static<Object>::operator*()
	{
		return m_Object;
	}

}

#pragma endregion