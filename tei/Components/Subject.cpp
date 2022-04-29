#include "teipch.h"
#include "Subject.h"
#include "ComponentHelpers.h"

components::Subject::ObserverHandle::ObserverHandle(std::weak_ptr<ObserverContainer> container, ObserverContainer::iterator at)
	: m_Container{ container }
	, m_Position{ at }
{}

tei::internal::components::Subject::ObserverHandle& tei::internal::components::Subject::ObserverHandle::operator=(ObserverHandle&& other)
{
	if (auto ptr{ std::exchange(m_Container, std::move(other.m_Container)).lock() })
		ptr->erase(m_Position);
	m_Position = std::move(other.m_Position);
	return *this;
}

bool tei::internal::components::Subject::ObserverHandle::Alive()
{
	return !m_Container.expired();
}

void tei::internal::components::Subject::ObserverHandle::Clear()
{
	*this = {};
}

void tei::internal::components::Subject::ObserverHandle::Detach()
{
	m_Container = {};
	m_Position = {};
}

components::Subject::ObserverHandle::~ObserverHandle()
{
	if (auto ptr{ m_Container.lock() })
		ptr->erase(m_Position);
}

components::Subject::Subject()
	: m_Observers{ std::make_shared<ObserverContainer>() }
{}

void components::Subject::RemoveObserver(ObserverHandle& handle)
{
	handle = {};
}

components::Subject::ObserverHandle components::Subject::AddObserver(ObserverContainer::value_type value)
{
	return { m_Observers, m_Observers->insert(std::move(value)) };
}

