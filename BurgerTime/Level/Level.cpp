#include "pch.h"
#include "Level.h"
#include "Hitbox.h"
#include "Score.h"

using namespace tei;
using namespace components;
using namespace unit;

tei::ecs::Object& Level::Create(tei::ecs::Object& parent, LevelData const& data)
{
	auto& object = parent.AddChild();

	auto& level = object.AddComponent<Level>();
	level.m_pObject = &object;
	level.m_pData = &data;
	
	object.AddComponent<Hitbox::List>();
	object.AddComponent<ScoreManager>(data);
	object.AddComponent<Score>();

	return object;
}

template <Level::LevelDataEvent::Type type, typename T>
Level::LevelDataEvent MakeDataEvent(T&& value)
{
	return Level::LevelDataEvent{ decltype(Level::LevelDataEvent::data){ std::in_place_index_t<size_t(type)>{}, std::forward<T>(value) }};
}

tei::ecs::Object& Level::CreateLayout(int id)
{
	m_pObject->Clear();

	m_ID = id;

	auto& layout = m_pData->levels[id];

	auto& layoutObject = m_pObject->AddChild();

	Position tileRoot{ layout.width / -2.f, layout.height / 2.f + 1 };
	Vec2 tileDiff{ 1, -1 };

	layoutObject.AddComponent<ObjectTransform>(Scale{ 1.f / (std::max(layout.width, layout.height) * .9f) });

	auto at = [&layout](int x, int y)
	{
		return layout.field[size_t(x + y * layout.width)];
	};

	auto add = [this, &layoutObject](StaticEntityData::Type type, ObjectTransform tf)
	{
		auto& data = m_pData->statics.at(type);
		auto& tile = layoutObject.AddChild();
		tile.AddComponent(std::move(tf));
		StaticEntity::Create(tile, data);
	};

	std::default_random_engine rng{ std::random_device{}() };

	auto getIngredientID = [&rng, count = m_pData->ingrendients.size()] (size_t index) -> size_t
	{
		if (index == 0)
			return 0;
		if (index == 2)
			return 1;
		else
			return rng() % (count - 2) + 2;
	};

	auto addIngredient = [this, &layoutObject, &rng] (ObjectTransform tf, size_t id) mutable
	{
		IngredientEnity::Create(layoutObject, m_pData->ingrendients[id]).AddComponent(std::move(tf));
	};

	using enum LevelLayoutData::TileType;

	std::vector<tei::unit::Position> enemyPoints{};
	tei::unit::Position playerPoint{};
	tei::unit::Position player2Point{};

	for (auto processedType : { LADDER, LADDER_PLATFORM, PLATFORM, LADDER_SHELF, SHELF })
	{
		for (int x{}; x < layout.width; ++x)
		{
			int ingredientID{};
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
						if (x == 0 || x == layout.width - 1)
							enemyPoints.push_back(pos);
						if (x == layout.width / 2)
						{
							if (y == 0)
								playerPoint = pos;
							if (y == 2)
								player2Point = pos;
						}
						break;

					case LADDER:
						add(StaticEntityData::LADDER, { pos });
						break;

					case LADDER_SHELF:
						add(StaticEntityData::LADDER, { pos });
						[[fallthrough]];
					case SHELF:
						add(StaticEntityData::PLATFORM, { pos });
						add(StaticEntityData::SHELF, { pos });
						addIngredient({ pos }, getIngredientID(ingredientID++) );
						break;
					}
				}
			}
		}
	}

	std::ranges::shuffle(enemyPoints, rng);

	for (int x{ 2 }; x < layout.width; x += 4)
	{
		add(StaticEntityData::PLATE, { Position{ tileRoot + tileDiff * Vec2{ x, layout.height + 2.5f } } });
	}

	// Enemies (tmp)
	{
		Anima::Create(layoutObject, m_pData->anima[0], playerPoint);

		auto pointIt{ enemyPoints.begin() };
		for ([[maybe_unused]] auto i : {1, 2, 3})
			Anima::Create(layoutObject, m_pData->anima[1], *pointIt++);

		for ([[maybe_unused]] auto i : {1, 2})
			Anima::Create(layoutObject, m_pData->anima[2], *pointIt++);
		
		if (m_pData->anima.size() > 3)
		for ([[maybe_unused]] auto i : {1})
			Anima::Create(layoutObject, m_pData->anima[3], *pointIt++);

		Anima::Create(layoutObject, m_pData->anima.back(), player2Point);
	}

	m_PlayerHealth = 3;
	m_PlayerAttacks = 4;

	Notify(LevelEvent{ .type = LevelEvent::LOADED, .levelID = m_ID });
	Notify(MakeDataEvent<LevelDataEvent::HEALTH>(m_PlayerHealth));
	Notify(MakeDataEvent<LevelDataEvent::ATTACKCOUNT>(m_PlayerAttacks));

	return layoutObject;
}

bool Level::DoPlayerAttack()
{
	bool allowed{ m_PlayerAttacks > 0 };
	m_PlayerAttacks = std::max<int>(m_PlayerAttacks - 1, 0);
	Notify(MakeDataEvent<LevelDataEvent::ATTACKCOUNT>(m_PlayerAttacks));
	return allowed;
}

bool Level::DoPlayerDeath()
{
	bool allowed{ m_PlayerHealth > 0 };
	m_PlayerHealth = std::max<int>(m_PlayerHealth - 1, 0);
	Notify(MakeDataEvent<LevelDataEvent::HEALTH>(m_PlayerHealth));
	return allowed;
}

