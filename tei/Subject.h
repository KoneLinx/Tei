#pragma once

#include <typeindex>
#include <map>
#include <tei/internal/ecs.h>

namespace tei::internal::components
{

	namespace detail
	{

		template <typename Observer, typename Ret, typename Event>
		constexpr std::type_identity<std::remove_cvref_t<Event>> EventTypeHelperFn(Ret(Observer::*)(Event))
		{
			return {};
		}

		template <typename Observer>
		decltype(EventTypeHelperFn(&Observer::operator())) EventTypeHelper(Observer&&)
		{
			return {}
		}

		template <typename>
		concept Event = true;

		template <typename Observer>
		concept Observer = requires (Observer observer)
		{
			{ EventTypeHelper(observer) } -> Event;
		};

	}

	class Subject
	{

	public:

		void Send(detail::Event auto);

		void AddObserver(detail::Observer auto = {});
		//void RemoveObserver(detail::Observer auto&);

	private:

		std::unordered_multimap<std::type_index, std::function<void(utility::AnyRef)>> m_Observers{};

	};

	void Subject::Send(detail::Event auto event)
	{
		for (auto& [typeindex, obsverver] : utility::SubrangeFromPair(m_Observers.equal_range(typeid(event))))
			obsverver(event);
	}

	inline void Subject::AddObserver(detail::Observer auto observer)
	{
		using Event = decltype(detail::EventTypeHelper(observer));
		m_Observers.insert({
			typeid(Event),
			[observer = std::move(observer)](utility::AnyRef event)
			{
				observer(event);
			}
		});
	}

	//inline void Subject::RemoveObserver(detail::Observer auto& observer)
	//{
	//	//auto possibleCommands{ utility::SubrangeFromPair(m_Observers.equal_range(typeid(detail::EventTypeHelper(observer)))) };
	//
	//}

}


namespace tei::external::components
{
	using tei::internal::components::Subject;
}