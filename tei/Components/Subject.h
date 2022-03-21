#pragma once

#include <any>
#include <typeindex>
#include <unordered_map>
#include <tei/internal/ecs.h>

namespace tei::internal::components
{
	class Subject;
}

void OnEnable(tei::internal::Internal, tei::internal::components::Subject&, tei::internal::ecs::Object const&);

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
		void Notify(Event const& = {}, bool ascend = true);

		template <detail::Observer Observer, detail::Event Event = detail::ObserverEvent_t<Observer>>
		Observer& AddObserver(Observer = {});

		template <detail::Observer Observer, detail::Event Event = detail::ObserverEvent_t<Observer>>
		void RemoveObserver(Observer const&);

		void RemoveObserver(utility::AnyRef const&);

	private:

		Subject* m_pParent{};

		std::unordered_multimap<std::type_index, std::pair<std::any, std::function<void(utility::AnyRef, std::any&)>>> m_Observers{};
		std::unordered_map<utility::AnyRef, decltype(m_Observers)::const_iterator> m_ObserverByData{};

		friend void ::OnEnable(tei::internal::Internal, tei::internal::components::Subject&, tei::internal::ecs::Object const&);

	};
	
	template <detail::Event Event>
	void Subject::Notify(Event const& event, bool ascend)
	{
		for (auto& [observer, handle] : utility::SubrangeFromPair(m_Observers.equal_range(typeid(event))) | std::views::values)
			handle(event, observer);
		if (ascend && m_pParent)
			m_pParent->Notify(event, ascend);
	}
	
	template<detail::Observer Observer, detail::Event Event>
	inline Observer& Subject::AddObserver(Observer observer)
	{
		auto it = m_Observers.insert({
			typeid(Event),
			{ 
				std::move(observer),
				[](utility::AnyRef event, std::any& observer)
				{
					std::any_cast<Observer&>(observer)(event.cast<Event>());
				}
			}
		});
		Observer& data{ std::any_cast<Observer&>(it->second.first) };
		m_ObserverByData.insert({ data, it });
		return data;
	}

	template<detail::Observer Observer, detail::Event Event>
	inline void Subject::RemoveObserver(Observer const& observer)
	{
		RemoveObserver(utility::AnyRef{ observer });
	}

}

namespace tei::external::components
{
	using tei::internal::components::Subject;
}
