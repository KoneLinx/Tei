#pragma once

#include <vector>
#include <memory>
#include <ranges>

namespace tei::internal::ecs
{

	class Component;

	class Object final
	{
	public:

		Object();
		~Object();

		Object(const Object & other) = delete;
		Object(Object && other) = delete;

		Object& operator = (const Object & other) = delete;
		Object& operator = (Object && other) = delete;

		template <std::derived_from<Component> Component>
		Component& AddComponent(Component = {});
		template <std::derived_from<Component> Component>
		Component& GetComponent() const;
		template <std::derived_from<Component> Component>
		void RemoveComponent();

		Object& AddChild();
		auto GetChildren() const;
		void RemoveChild(Object&);

		void Update();
		void Render() const;

	private:

		std::vector<std::pair<std::type_info const*, std::unique_ptr<Component>>> m_Components;
		std::vector<std::unique_ptr<Object>> m_Children;

		Component& AddComponent(std::type_info const*, Component*);
		decltype(m_Components)::const_iterator GetComponent(std::type_info const*) const;
		void RemoveComponent(std::type_info const*);

	};

	template<std::derived_from<Component> Component>
	inline Component& Object::AddComponent(Component component)
	{
		return static_cast<Component&>(Object::AddComponent(&typeid(Component), new Component{ std::move(component) }));
	}

	template<std::derived_from<Component> Component>
	inline Component& Object::GetComponent() const
	{
		return static_cast<Component&>(*Object::GetComponent(&typeid(Component))->second);
	}

	template<std::derived_from<Component> Component>
	inline void Object::RemoveComponent()
	{
		Object::RemoveComponent(&typeid(Component));
	}

	inline auto Object::GetChildren() const
	{
		return std::views::transform(m_Children, [] (auto const& uptr) -> auto& { return *uptr; });
	}

}

namespace tei::external::ecs
{
	using Object = tei::internal::ecs::Object;
}