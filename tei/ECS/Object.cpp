
#include "Object.h"

#include <algorithm>

using namespace tei::internal::ecs;

Object::Object(bool active)
	: m_Components{}
	, m_Children{}
	, m_Active{ false }
	, m_SetActive{ active }
	, m_Initialised{}
{}

Object::~Object()
{
	Do(Message::CLEANUP);
}

void Object::Activate(bool state) noexcept
{
	m_SetActive = state;
}

Object& Object::AddChild(bool active)
{
	METRICS_TIMEBLOCK;
	return m_Children.emplace_back(active);
}

void Object::AddComponent(Component<>* pComp, Component<>::Handle pHandle)
{	
	METRICS_TIMEBLOCK;
	m_Components.push_back({
		std::unique_ptr<Component<>>{ pComp },
		pHandle
	});
}

Object::Component<>& Object::GetComponent(Component<>::Handle handle) const
{
	METRICS_TIMEBLOCK;
	auto const it{ std::ranges::find(m_Components, handle, utility::tuple_index_projector<1>{}) };
	if (it == m_Components.end())
		throw utility::TeiRuntimeError{ "No such component", typeid(std::to_address(it)).name() };
	return *it->first;
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
		if (!m_SetActive)
			return;
		if (!m_Initialised)
			Do(INIT);
		m_Active = true;
	}
	break;
	case UPDATE:
	{
		if (m_Active != m_SetActive)
			Do(m_SetActive ? ENABLE : DISABLE);
		if (!m_Active)
			return;
	}
	break;
	}

	for (auto& [pComp, pHandle] : m_Components)
		pHandle(*pComp, message);

	// TODO move bools out of object on stack into cache!

	for (auto& child : m_Children)
		if (child.m_Active || message == ENABLE || message == CLEANUP || message == UPDATE)
			child.Do(message);

	switch (message)
	{
	case INIT:
	{
		m_Initialised = true;
	}
	break;
	case CLEANUP:
	{
		m_Active = false;
		m_SetActive = false;
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