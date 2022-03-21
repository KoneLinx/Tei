#include "Subject.h"
#include "ComponentHelpers.h"

void tei::internal::components::Subject::RemoveObserver(utility::AnyRef const& ref)
{
	auto it = m_ObserverByData.find(ref);
	if (it != std::ranges::end(m_ObserverByData))
	{
		m_Observers.erase(it->second);
		m_ObserverByData.erase(it);
	}
	else throw utility::TeiRuntimeError{ "No such component" };
}

void OnEnable(tei::internal::Internal, tei::internal::components::Subject& subject, tei::internal::ecs::Object const& object)
{
	subject.m_pParent = tei::internal::components::FindParentComponent<tei::internal::components::Subject>(object);
}
