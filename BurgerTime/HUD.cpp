#include "pch.h"
#include "HUD.h"

using tei::components::ObjectTransform;

void IconbarDisplay::LoadLevel(tei::ecs::Object& object)
{

	auto& level10object = object.AddChild();
	auto& level5object = level10object.AddChild();
	auto& level1object = level5object.AddChild();

	auto makeChild = [](auto& object, int idx, auto texture) -> auto&
	{
		auto& child = object.AddChild();
		child.AddComponent<ObjectTransform>(tei::unit::Position{ 0, texture->size.y * idx });
		child.AddComponent<tei::components::TextureRenderComponent>();
		child.AddComponent(std::move(texture));
		return child;
	};

	auto texture1 = tei::Resource<tei::resource::Texture>();
	auto texture5 = tei::Resource<tei::resource::Texture>();
	auto texture10 = tei::Resource<tei::resource::Texture>();

	makeChild(level10object, 0, texture10);

	makeChild(level5object, 0, texture5);
	
	makeChild(level1object, 0, texture1);
	makeChild(level1object, 1, texture1);
	makeChild(level1object, 2, texture1);
	makeChild(level1object, 3, texture1);

	m_Update = [&](int levelID)
	{

		for (int idx{}; auto & child : level1object.GetAllChildren())
		{
			child.SetState(++idx <= (levelID % 5));
		}

		if (auto& child5 = level5object.GetAllChildren().front(); levelID / 5 % 2)
		{
			child5.SetState(true);
			level1object.GetComponent<ObjectTransform>().get().position.y = texture5->size.y;
		}
		else
		{
			child5.SetState(false);
			level1object.GetComponent<ObjectTransform>().get().position.y = 0;
		}

		if (auto& child10 = level10object.GetAllChildren().front(); levelID / 10 % 2)
		{
			child10.SetState(true);
			level5object.GetComponent<ObjectTransform>().get().position.y = texture10->size.y;
		}
		else
		{
			child10.SetState(false);
			level5object.GetComponent<ObjectTransform>().get().position.y = 0;
		}

	};

}

void IconbarDisplay::OnInitialize(tei::ecs::Object& object)
{
}

void IconbarDisplay::LoadHealth(tei::ecs::Object& object)
{

	auto makeChild = [](auto& object, int idx, auto texture) -> auto&
	{
		auto& child = object.AddChild();
		child.AddComponent<ObjectTransform>(tei::unit::Position{ 0, texture->size.y * idx });
		child.AddComponent<tei::components::TextureRenderComponent>();
		child.AddComponent(std::move(texture));
		return child;
	};

	auto& healthobject = object.AddChild();

	auto texture = tei::Resource<tei::resource::Texture>();

	makeChild(healthobject, 0, texture);
	makeChild(healthobject, 1, texture);
	makeChild(healthobject, 2, texture);

	m_Update = [&](int health)
	{
		for (int idx{}; auto & child : healthobject.GetAllChildren())
		{
			child.SetState(++idx <= health);
		}
	};

}
