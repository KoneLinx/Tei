#pragma once

#include <tuple>
#include <typeinfo>
#include <tei/internal/ecs.h>

namespace tei::internal::components
{

	template <typename ... Components>
	class Refs
	{
	public:

		constexpr Refs() noexcept
			: m_Data{ *static_cast<Components*>(nullptr) ... }
		{}

		template <typename Component>
		constexpr Component& get() const noexcept
		{
			return std::get<std::reference_wrapper<Component>>(as_tuple());
		}
		
		template <size_t INDEX>
		constexpr auto& get() const noexcept
		{
			return std::get<INDEX>(as_tuple()).get();
		}

		constexpr std::tuple<std::reference_wrapper<Components>...>& as_tuple()
		{
			return m_Data;
		}
		constexpr std::tuple<std::reference_wrapper<Components>...> const& as_tuple() const
		{
			return m_Data;
		}

		std::tuple<std::reference_wrapper<Components>...> m_Data;

	};

	template <size_t N>
	struct Name
	{
		constexpr Name(char const (&name)[N + 1]) noexcept
		{
			for (size_t i{}; i < N; ++i)
				_name[i] = name[i];
		}
		char _name[N]{};
	};

	template <size_t N>
	Name(char const (&)[N]) -> Name<N - 1>;

	template <Name = "", typename ... Components>
	class RefComponent
	{
	public:

		Refs<Components...> refs{};

	};

}

namespace tei::external::components
{
	using tei::internal::components::RefComponent;
}

template <typename ... Components>
struct std::tuple_size<tei::internal::components::Refs<Components...>> : std::integral_constant<size_t, sizeof...(Components)>
{};

template <size_t INDEX, typename ... Components>
struct std::tuple_element<INDEX, tei::internal::components::Refs<Components...>> : std::tuple_element<INDEX, std::tuple<Components&...>>
{};


template <tei::internal::components::Name NAME, typename ... Components>
static inline void OnEnable(tei::internal::Internal, tei::internal::components::RefComponent<NAME, Components...>& refcomp, tei::internal::ecs::Object& object)
{
	auto const getter = [&object] <typename Component> (std::reference_wrapper<Component>& ref)
	{
		if (auto* pData = object.HasComponent<Component>())
		{
			ref = *pData;
		}
		else
		{
			if constexpr (std::default_initializable<Component>)
				ref = object.AddComponent<Component>();
			else
				throw tei::internal::utility::TeiRuntimeError{ "Cannot find or create required component", typeid(Component).name() };
		}
	};
	(getter(std::get<std::reference_wrapper<Components>>(refcomp.refs.as_tuple())), ...);
}
