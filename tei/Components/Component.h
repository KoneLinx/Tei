#pragma once

namespace tei::internal::ecs
{

	class Object;

	class Component
	{
	public:

		Component() = default;
		virtual ~Component() = default;

		virtual Object& Parent() const noexcept final;
	
	private:

		friend Object;

		virtual void Update() {};
		virtual void Render() const {};

		virtual void SetParent(Object&) noexcept final; 

		Object* m_Parent{};

	};

	inline Object& Component::Parent() const noexcept
	{
		return *m_Parent;
	}

	inline void Component::SetParent(Object& object) noexcept
	{
		m_Parent = std::addressof(object);
	}

}

namespace tei::external::ecs
{
	using Component = tei::internal::ecs::Component;
}