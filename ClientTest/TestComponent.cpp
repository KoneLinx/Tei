#include "TestComponent.h"

#include <iostream>

template<>
void On<tei::ecs::Message::INIT>(std::string_view name)
{
	std::cout << "Initialised " << name << std::endl;
}

template<>
void On<tei::ecs::Message::ENABLE>(std::string_view name)
{
	std::cout << "Enabled " << name << std::endl;
}
template<>
void On<tei::ecs::Message::UPDATE>(std::string_view name)
{
	std::cout << "Updated " << name << std::endl;
}
template<>
void On<tei::ecs::Message::RENDER>(std::string_view name)
{
	std::cout << "Rendered " << name << std::endl;
}
template<>
void On<tei::ecs::Message::DISABLE>(std::string_view name)
{
	std::cout << "Disabled " << name << std::endl;
}
template<>
void On<tei::ecs::Message::CLEANUP>(std::string_view name)
{
	std::cout << "Cleaned " << name << " up" << std::endl;
}