#include "InputComponent.h"

void tei::internal::input::detail::InputComponent::Clear()
{
	for (auto command : m_Commands)
		Input->RemoveCommand(command);
	m_Commands.clear();
}

tei::internal::input::detail::InputComponent::~InputComponent()
{
	Clear();
}

void OnDisable(std::nullptr_t, tei::internal::input::detail::InputComponent& comp)
{
	comp.Clear();
}