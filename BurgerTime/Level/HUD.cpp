#include "pch.h"
#include "HUD.h"

using tei::components::ObjectTransform;

tei::ecs::Object& IconbarDisplay::CreateLevelDisplay(tei::ecs::Object& parent, LevelData const& leveldata)
{
	auto& object = parent.AddChild();

	auto& self = object.AddComponent<IconbarDisplay>();

	auto& level10object = object.AddChild();
	auto& level5object = level10object.AddChild();
	auto& level1object = level5object.AddChild();

	level1object.AddComponent<ObjectTransform>();
	level5object.AddComponent<ObjectTransform>();
	level10object.AddComponent<ObjectTransform>();

	auto makeChild = [](auto& object, int idx, auto texture) -> auto&
	{
		auto& child = object.AddChild();
		child.AddComponent<ObjectTransform>(tei::unit::Position{ 0, /*texture->size.y **/ idx * 1.1f });
		child.AddComponent<tei::components::TextureRenderComponent>();
		child.AddComponent(std::move(texture));
		return child;
	};

	auto& texture1 = leveldata.particles.at(StaticEntityData::LEVEL_1).texture;
	auto& texture5 = leveldata.particles.at(StaticEntityData::LEVEL_5).texture;
	auto& texture10 = leveldata.particles.at(StaticEntityData::LEVEL_10).texture;

	auto& child10 = makeChild(level10object, 0, texture10);

	auto& child5 = makeChild(level5object, 0, texture5);
	
	makeChild(level1object, 0, texture1);
	makeChild(level1object, 1, texture1);
	makeChild(level1object, 2, texture1);
	makeChild(level1object, 3, texture1);

	self.m_Update = [&](int levelID)
	{
		levelID += 1;

		for (int idx{}; auto & child : level1object.GetAllChildren())
		{
			child.SetState(++idx <= (levelID % 5));
		}

		if (levelID / 5 % 2)
		{
			child5.SetState(true);
			level1object.GetComponent<ObjectTransform>().get().position.y = 1;
		}
		else
		{
			child5.SetState(false);
			level1object.GetComponent<ObjectTransform>().get().position.y = 0;
		}

		if (levelID / 10 % 2)
		{
			child10.SetState(true);
			level5object.GetComponent<ObjectTransform>().get().position.y = 1;
		}
		else
		{
			child10.SetState(false);
			level5object.GetComponent<ObjectTransform>().get().position.y = 0;
		}

	};

	auto& level = parent.GetComponent<Level>();
	
	level.AddObserver(
		[&self](Level::LevelEvent const& event)
		{
			if (event.type == event.LOADED)
			{
				self.Update(event.levelID);
			}
		}
	).Detach();

	return object;
}

void IconbarDisplay::Update(int value)
{
	m_Update(value);
}

tei::ecs::Object& IconbarDisplay::CreateHealthDisplay(tei::ecs::Object& parent, LevelData const& leveldata)
{
	auto& object = parent.AddChild();
	auto& self = object.AddComponent<IconbarDisplay>();
	auto& healthobject = object.AddChild();

	auto& texture = leveldata.particles.at(StaticEntityData::HEARTH).texture;

	for (size_t i{}; i < 3; ++i)
	{
		auto& child = healthobject.AddChild();
		child.AddComponent<ObjectTransform>(tei::unit::Position{ 0, /*texture->size.y **/ i * 1.1f });
		child.AddComponent<tei::components::TextureRenderComponent>();
		child.AddComponent(std::move(texture));
	}

	self.m_Update = [&](int health)
	{
		for (int idx{}; auto & child : healthobject.GetAllChildren())
		{
			child.SetState(++idx <= health);
		}
	};

	auto& level = parent.GetComponent<Level>();
	
	level.AddObserver(
		[&self](Level::LevelDataEvent const& data)
		{
			if (data.data.index() == data.HEALTH)
			{
				self.Update(std::get<data.HEALTH>(data.data));
			}
		}
	).Detach();

	return object;
}
