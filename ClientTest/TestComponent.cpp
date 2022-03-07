#include "TestComponent.h"

#include <iostream>

void OnInitialize(std::string& name, tei::ecs::Object& parent)
{
	if (auto& root = parent.GetParent(); &root != nullptr)
	{
		auto it = std::ranges::find(root.GetAllChildren(), &name, [](auto& obj) { return &obj.GetComponent<std::string>(); });
		name = root.GetComponent<std::string>() + " > Child-" + std::to_string(std::distance(root.GetAllChildren().begin(), it));
	}
	else
		name = "Scene";
}

void OnEnable(std::string_view name)
{
	std::cout << "Enabled: " << name << std::endl;
}
void OnUpdate(std::string_view name)
{
	std::cout << "Updated: " << name << std::endl;
}

void OnRender(std::string_view name)
{
	std::cout << "Rendered: " << name << std::endl;
}

void OnDisable(std::string_view name)
{
	std::cout << "Disabled: " << name << std::endl;
}

void OnCleanup(std::string_view name)
{
	std::cout << "Cleaning: " << name << std::endl;
}