#pragma once

#include <any>
#include <typeindex>
#include <functional>
#include <unordered_map>
#include <tei/utility.h>

namespace tei::internal::components
{
	class Subject;
}
namespace tei::internal::ecs
{
	class Object;
}

namespace tei::internal::components
{

	namespace detail
	{
		
		template <typename Ret, typename Event>
		std::remove_cvref_t<Event> EventTypeDetector(Ret(*)(Event));

		template <typename Observer, typename Ret, typename Event>
		std::remove_cvref_t<Event> EventTypeDetector(Ret(Observer::*)(Event));
		
		template <typename Observer, typename Ret, typename Event>
		std::remove_cvref_t<Event> EventTypeDetector(Ret(Observer::*)(Event) const);

		template <typename Observer>
		decltype(EventTypeDetector(&Observer::operator())) EventTypeDetector(Observer&&);

		template <typename Observer>
		using ObserverEvent_t = decltype(EventTypeDetector(std::declval<Observer>()));

		template <typename>
		concept Event = true;

		template <typename Observer_t>
		concept Observer = Event<ObserverEvent_t<Observer_t>>;

	}

	class Subject
	{

		class ObserverHandle;

	public:

		Subject();
		Subject(Subject const&) = delete;
		Subject(Subject &&) = default;
		Subject& operator = (Subject const&) = delete;
		Subject& operator = (Subject &&) = default;
		
		template <detail::Event Event>
		void Notify(Event const& = {});
		
		template <detail::Observer Observer, detail::Event Event = detail::ObserverEvent_t<Observer>>
		[[nodiscard]] ObserverHandle AddObserver(Observer = {});

		static void RemoveObserver(ObserverHandle&);

	private:

		using ObserverContainer = std::unordered_multimap<std::type_index, std::function<void(utility::AnyReference event)>>;

		ObserverHandle AddObserver(ObserverContainer::value_type);

		std::shared_ptr<ObserverContainer> m_Observers;

		class ObserverHandle
		{
		public:

			ObserverHandle() = default;
			~ObserverHandle();

			ObserverHandle(ObserverHandle &&) = default;
			ObserverHandle& operator = (ObserverHandle &&);
			
			ObserverHandle(ObserverHandle const&) = delete;
			ObserverHandle& operator = (ObserverHandle const&) = delete;

			bool Alive();
			void Clear();
			void Detach();

		private:

			friend Subject;

			ObserverHandle(std::weak_ptr<ObserverContainer>, ObserverContainer::iterator);

			std::weak_ptr<ObserverContainer> m_Container{};
			ObserverContainer::iterator m_Position{};
		};

	};

	template<detail::Event Event>
	inline void Subject::Notify(Event const& event)
	{
		for (auto& observer : utility::SubrangeFromPair(m_Observers->equal_range(typeid(event))) | std::views::values)
			observer(event);
	}

	template<detail::Observer Observer, detail::Event Event>
	inline Subject::ObserverHandle Subject::AddObserver(Observer observer)
	{
		return AddObserver({
			typeid(Event),
			[observer = std::move(observer)] (utility::AnyReference event)
			{
				observer(event);
			}
		});
	}

}

namespace tei::external::components
{
	using tei::internal::components::Subject;
}