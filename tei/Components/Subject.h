#pragma once

#include <any>
#include <typeindex>
#include <unordered_map>
#include <tei/internal/ecs.h>

namespace tei::internal::components
{

	namespace detail
	{

		template <typename Observer, typename Ret, typename Event>
		std::remove_cvref_t<Event> EventTypeDetector(Ret(Observer::*)(Event));
		
		template <typename Observer, typename Ret, typename Event>
		std::remove_cvref_t<Event> EventTypeDetector(Ret(Observer::*)(Event) const);

		template <typename Observer>
		using ObserverEvent_t = decltype(EventTypeDetector(&Observer::operator()));

		template <typename>
		concept Event = true;

		template <typename Observer_t>
		concept Observer = Event<ObserverEvent_t<Observer_t>>;

	}

	class Subject
	{

	public:

		template <detail::Event Event>
		void Notify(Event const& = {});

		template <detail::Observer Observer, detail::Event Event = detail::ObserverEvent_t<Observer>>
		void AddObserver(Observer = {});

	private:

		std::unordered_multimap<std::type_index, std::function<void(utility::AnyRef)>> m_Observers{};

	};
	
	template <detail::Event Event>
	void Subject::Notify(Event const& event)
	{
		for (auto& [typeindex, obsverver] : utility::SubrangeFromPair(m_Observers.equal_range(typeid(event))))
			obsverver(event);
	}
	
	template<detail::Observer Observer, detail::Event Event>
	inline void Subject::AddObserver(Observer observer)
	{
		auto const handle{
			[observer = std::move(observer)] (utility::AnyRef event) mutable
			{
				observer(event.cast<Event>());
			}
		};
		m_Observers.emplace(
			typeid(Event),
			std::move(handle)
		);
	}

}

namespace tei::external::components
{
	using tei::internal::components::Subject;
}