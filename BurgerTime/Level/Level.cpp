#include "pch.h"
#include "Level.h"
#include "Hitbox.h"

using namespace tei;
using namespace components;
using namespace unit;

tei::ecs::Object& Level::Create(tei::ecs::Object& parent, LevelData const& data)
{
	auto& object = parent.AddChild();

	object.AddComponent<ObjectTransform>();
	auto& level = object.AddComponent<Level>();
	level.m_pData = &data;
	level.CreateLayout(object, 2);
	
	object.AddComponent<Hitbox::List>();

	return object;
}

void Level::CreateLayout(tei::ecs::Object& object, int id)
{
	for (auto& child : object.GetAllChildren())
		object.RemoveChild(child);

	m_ID = id;

	auto& layout = m_pData->levels[id];

	auto& layoutObject = object.AddChild();

	Position tileRoot{ layout.width / -2.f, layout.height / 2.f + 1 };
	Vec2 tileDiff{ 1, -1 };

	object.GetComponent<ObjectTransform>().get().scale = Scale{ 1.f / (std::max(layout.width, layout.height) * .9f) };

	auto at = [&layout](int x, int y)
	{
		return layout.field[size_t(x + y * layout.width)];
	};

	auto add = [this, &layoutObject](StaticEntityData::Type type, ObjectTransform tf)
	{
		auto& data = *std::ranges::find(
			m_pData->other,
			type,
			[](StaticEntityData const& data) -> auto const&
			{
				return data.type;
			}
		);
		auto& tile = layoutObject.AddChild();
		tile.AddComponent(std::move(tf));
		StaticEntity::Create(tile, data);
	};

	auto addIngredient = [this, &layoutObject, i = 0] (ObjectTransform tf) mutable
	{
		IngredientEnity::Create(layoutObject, m_pData->ingrendients[i++ % 6]).AddComponent(std::move(tf));
	};

	using enum LevelLayoutData::TileType;

	for (auto processedType : { LADDER, LADDER_PLATFORM, PLATFORM, LADDER_SHELF, SHELF })
	{
		for (int x{}; x < layout.width; ++x)
		{
			for (int y{}; y < layout.height; ++y)
			{
				auto type = at(x, y);
				if (type == processedType)
				{
					Position pos{ tileRoot + tileDiff * Vec2{ x, y } };
					switch (type)
					{
					case LADDER_PLATFORM:
						add(StaticEntityData::LADDER, { pos });
						[[fallthrough]];
					case PLATFORM:
						add(StaticEntityData::PLATFORM, { pos });
						break;

					case LADDER:
						add(StaticEntityData::LADDER, { pos });
						break;

					case LADDER_SHELF:
						add(StaticEntityData::LADDER, { pos });
						[[fallthrough]];
					case SHELF:
						add(StaticEntityData::SHELF, { pos });
						addIngredient({ pos });
						break;
					}
				}
			}
		}
	}

	for (int x{ 2 }; x < layout.width; x += 4)
	{
		add(StaticEntityData::PLATE, { Position{ tileRoot + tileDiff * Vec2{ x, layout.height + 2.5f } } });
	}

}
