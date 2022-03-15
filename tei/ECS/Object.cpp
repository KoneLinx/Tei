
#include "Object.h"

#include <algorithm>

#include <cassert>

using namespace tei::internal::ecs;

Object::Object(Object* pParent, bool active)
	: m_pParent{ pParent }
	, m_Components{}
	, m_Children{}
	, m_Active{ false }
	, m_State{ active }
	, m_Initialised{}
{}

Object::~Object()
{
	Do(Message::CLEANUP);
}

void Object::SetState(bool state) 
{
	m_State = state;
}

Object& Object::AddChild(bool active)
{
	METRICS_TIMEBLOCK;
	return *m_Children.emplace_back(new Object{ this, active });
}

void Object::RemoveChild(Object const& child)
{
	auto it{ std::ranges::find(m_Children, &child, utility::wrapped_ptr_projector{})};
	m_Children.erase(it);
}

void Object::AddComponent(std::type_index type, Component<>* pComp, Handle pHandle)
{	
	METRICS_TIMEBLOCK;
	m_Components.push_back({
		type,
		std::unique_ptr<Component<>>{ pComp },
		pHandle
	});
	pHandle(*pComp, Message::INIT, *this);
}

typename decltype(Object::m_Components)::const_iterator Object::GetComponent(std::type_index type) const
{
	METRICS_TIMEBLOCK;
	return std::ranges::find(m_Components, type, utility::tuple_index_projector<0>{});
}

std::unique_ptr<Object::Component<>> tei::internal::ecs::Object::ExtractComponent(std::type_index type)
{
	auto constIt{ GetComponent(type) };
	if (constIt == m_Components.cend())
		throw utility::TeiRuntimeError{ "No such component found in object" };
	auto it{ m_Components.begin() + std::distance(m_Components.cbegin(), constIt) };
	auto uptr{ std::move(std::get<1>(*it)) };
	m_Components.erase(it);
	return uptr;
}

void Object::Do(Message message)
{
	METRICS_TIMEBLOCK;

	using enum Message;
	switch (message)
	{
	case CLEANUP:
	{
		if (!m_Initialised)
			return;
		if (m_Active)
			Do(DISABLE);
	}
	break;
	case ENABLE:
	{
		if (!m_State)
			return;
		if (!m_Initialised)
			Do(INIT);
	}
	break;
	case UPDATE:
	{
		if (m_Active != m_State)
			Do(m_State ? ENABLE : DISABLE);
		if (!m_Active)
			return;
	}
	break;
	case RENDER:
	{
		if (!m_Active)
			return;
	}
	break;
	}

	for (auto& [type, pComp, handle] : utility::RangePerIndex(m_Components)) // False intelisense error
		handle(*pComp, message, *this);

	for (auto& child : m_Children)
		if (child || message == ENABLE || message == CLEANUP || message == UPDATE)
			child->Do(message);

	switch (message)
	{
	case ENABLE:
	{
		m_Active = true;
	}
	break;
	case INIT:
	{
		m_Initialised = true;
	}
	break;
	case CLEANUP:
	{
		m_Active = false;
		m_State = false;
		m_Initialised = false;
		m_Children.clear();
		m_Components.clear();
	}
	break;
	case DISABLE:
	{
		m_Active = false;
	}
	break;
	}
}

Object::Object(Object const& other)
	: Object{ nullptr, other.m_State }
{	
	m_Initialised = other.m_Initialised;

	m_Components.reserve(std::ranges::size(other.m_Components));
	using cref_type = std::ranges::range_reference_t<decltype(std::as_const(m_Components))>;
	using value_type = std::ranges::range_value_t<decltype(m_Components)>;
	std::ranges::transform(
		other.m_Components,
		std::back_inserter(m_Components),
		[] (cref_type val) -> value_type
		{
			auto const& [type, pValue, handle] = val;
			return { type, pValue->Clone(), handle };
		}
	);

	std::ranges::transform(
		other.m_Children,
		std::back_inserter(m_Children),
		[] (Object const& obj) -> std::unique_ptr<Object>
		{
			return std::unique_ptr<Object>{ new Object{ obj } };
		},
		utility::wrapped_ptr_projector<true>{}
	);
}
