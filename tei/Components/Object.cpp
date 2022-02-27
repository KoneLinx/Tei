
#include "Object.h"
#include "Component.h"

#include <cassert>

namespace tei::internal::ecs
{

	constexpr static auto getFirst =
	[] (auto const& pair)
	{ 
		return pair.first; 
	};

	Component& Object::AddComponent(std::type_info const* pId, Component* pComponent)
	{
		pComponent->SetParent(*this);
		return *m_Components.emplace_back(pId, static_cast<Component*>(pComponent)).second;
	}

	decltype(Object::m_Components)::const_iterator Object::GetComponent(std::type_info const* pId) const
	{
		auto const it{ std::ranges::find(m_Components, pId, getFirst) };
		assert(it != m_Components.end() && "No such component");
		return it;
	}

	void Object::RemoveComponent(std::type_info const* pId)
	{
		m_Components.erase(GetComponent(pId));
	}

	Object::Object()
		: m_Components{}
		, m_Children{}
	{}

	Object::~Object()
	{}

	Object& Object::AddChild()
	{
		return *m_Children.emplace_back(std::make_unique<Object>());
	}

	void Object::RemoveChild(Object& object)
	{
		auto const it{ std::ranges::find(m_Children, &object, [](auto const& uptr) { return std::to_address(uptr); }) };
		assert(it != m_Children.end() && "No such child");
		m_Children.erase(it);
	}

	void Object::Update()
	{
		for (auto& [pId, pComponent] : m_Components)
			pComponent->Update();
		for (auto& pChild : m_Children)
			pChild->Update();
	}

	void Object::Render() const
	{
		for (auto& pChild : m_Children)
			pChild->Render();
		for (auto& [pId, pComponent] : m_Components)
			pComponent->Render();
	}

}