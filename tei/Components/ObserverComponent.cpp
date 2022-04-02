#include "ObserverComponent.h"

void tei::internal::components::ObserverComponent::Clear()
{
	for (auto& [subject, observers] : m_Observers)
		for (auto observer : observers)
			subject->RemoveObserver(observer);
	m_Observers.clear();
}

void OnDisable(std::nullptr_t, tei::internal::components::ObserverComponent& comp)
{
	comp.Clear();
}
