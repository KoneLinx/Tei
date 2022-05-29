#include "teipch.h"
#include "Object.h"

#include <algorithm>
#include <cassert>
#include <ranges>

using namespace tei::internal::ecs;
using namespace tei::internal;
namespace r = std::ranges;
namespace v = std::views;

Object::Object(Object* pParent, bool active)
	: m_pParent{ pParent }
	, m_Components{}
	, m_Children{}
	, m_Active{ false }
	, m_State{ active }
	, m_Initialised{}
	, m_Clear{}
{}

Object::~Object()
{
	METRICS_TIMEBLOCK;

	Do(Message::CLEANUP);
}

void Object::SetState(bool state) 
{
	m_State = state;
}

Object& Object::AddChild(bool active)
{
	METRICS_TIMEBLOCK;

	//if (m_Active)
	//	throw utility::TeiRuntimeError{ "Cannot modify object list while active" };

	return *m_Children.emplace_back(new Object{ this, active });
}

void Object::RemoveChild(Object const& child)
{
	METRICS_TIMEBLOCK;

	if (m_Active)
		throw utility::TeiRuntimeError{ "Cannot modify object list while active" };

	auto it{ std::ranges::find(m_Children, &child, utility::projectors::to_address{})};
	m_Children.erase(it);
}

void tei::internal::ecs::Object::Clear()
{
	METRICS_TIMEBLOCK;

	if (m_Active)
		throw utility::TeiRuntimeError{ "Cannot modify object list while active" };

	Do(Message::CLEANUP);
}

void tei::internal::ecs::Object::ClearChildren()
{
	METRICS_TIMEBLOCK;

	if (m_Active)
		throw utility::TeiRuntimeError{ "Cannot modify object list while active" };

	m_Children.clear();
}

void Object::AddComponentImpl(std::type_info const& type, ComponentBase* pComp)
{	
	METRICS_TIMEBLOCK;

	if (m_Active)
		throw utility::TeiRuntimeError{ "Cannot modify object component list while active" };

	m_Components.push_back({ type, std::unique_ptr<ComponentBase>{ pComp } });
	if (m_Initialised)
		pComp->Do(Message::INIT, *this);
}

Object::ComponentBase* Object::GetComponentImpl(std::type_info const& type) const
{
	METRICS_TIMEBLOCK;
	auto it{ r::find(m_Components | v::keys, type).base() };
	if (it == r::end(m_Components))
		return {};
	else
		return std::to_address(it->second);
}

std::unique_ptr<Object::ComponentBase> tei::internal::ecs::Object::ExtractComponentImpl(std::type_info const& type)
{
	METRICS_TIMEBLOCK;

	if (m_Active)
		throw utility::TeiRuntimeError{ "Cannot modify object list while active" };

	auto it{ r::find(m_Components | v::keys, type).base() };
	if (it == r::end(m_Components))
		ExceptComponentNotFound(type);

	auto uptr{ std::move(it->second) };
	m_Components.erase(it);

	if (m_Initialised)
		uptr->Do(Message::CLEANUP, *this);

	return uptr;
}

template <typename Message>
void DoCall(Object& self, auto& components, auto& children, bool force = false)
{
	//for (auto& [type, pComp] : utility::RangePerIndex(components)) // occasional crash, not figured out yet. May be due to internal transform_view caching
	//	pComp->Do(Message{}, self);
	for (size_t i{}; i < std::ranges::size(components); ++i)
	{
		components[i].second->Do(Message{}, self);
	}

	for (auto& child : utility::RangePerIndex(children))
		if (child && (force || child->IsActive()))
			child->Do(Message{});
}

void Object::Do(Message::Init)
{
	METRICS_TIMEBLOCK;

	if (m_Initialised)
		return;

	DoCall<Message::Init>(*this, m_Components, m_Children);

	m_Initialised = true;
}

void Object::Do(Message::Cleanup)
{
	METRICS_TIMEBLOCK;

	if (!m_Initialised)
		return;

	if (m_Active)
		Do(Message::DISABLE);

	DoCall<Message::Cleanup>(*this, m_Components, m_Children, true);
	
	m_Active = false;
	m_State = false;
	m_Initialised = false;
	m_Clear = false;
	m_Children.clear();
	m_Components.clear();
}

void Object::Do(Message::Enable)
{
	METRICS_TIMEBLOCK;

	if (!m_State)
		return;

	if (!m_Initialised)
		Do(Message::INIT);

	DoCall<Message::Enable>(*this, m_Components, m_Children, true);
	
	m_Active = true;
}

void Object::Do(Message::Disable)
{
	METRICS_TIMEBLOCK;

	m_Active = false;

	DoCall<Message::Disable>(*this, m_Components, m_Children);
}

void Object::Do(Message::Update)
{
	METRICS_TIMEBLOCK;

	if (m_Clear)
		return Do(Message::CLEANUP);

	if (m_Active != m_State)
		if (m_State)
			Do(Message::ENABLE);
		else
			Do(Message::DISABLE);

	if (!m_Active)
		return;

	DoCall<Message::Update>(*this, m_Components, m_Children, true);
}

void Object::Do(Message::FixedUpdate)
{
	METRICS_TIMEBLOCK;

	DoCall<Message::FixedUpdate>(*this, m_Components, m_Children);
}

void Object::Do(Message::Render)
{
	METRICS_TIMEBLOCK;

	DoCall<Message::Render>(*this, m_Components, m_Children);
}

void* Object::ExceptComponentNotFound(std::type_info const& type)
{
	throw utility::TeiRuntimeError{ "No such component found in object", type.name() };
}

void Object::ComponentBase::ExceptCannotCopy(std::type_info const& type)
{
	throw utility::TeiRuntimeError{ "Cannot copy object component", type.name() };
}
