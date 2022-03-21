#include "InputComponent.h"

void tei::internal::input::detail::InputComponent::Clear()
{
	for (auto command : m_Commands)
		Input->RemoveCommand(command);
	m_Commands.clear();
}

void OnDisable(tei::internal::Internal, tei::internal::input::detail::InputComponent& comp)
{
	comp.Clear();
}