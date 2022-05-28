#include "pch.h"
#include "Level.h"
#include "Hitbox.h"
#include "Score.h"
#include "HUD.h"
#include "Particle.h"
#include "Player.h"
#include "../Loader/LevelLoader.h"

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
	if (m_pLayout)
		m_pObject->RemoveChild(*m_pLayout);

	m_ID = id;

	if (id >= size(m_pData->levels))
	{
		Notify(LevelEvent{ LevelEvent::ALL_COMPLETED, id });
		return *m_pObject;
	}

	auto& layout = m_pData->levels[id];
	
	auto& layoutObject = m_pObject->AddChild();
	m_pLayout = &layoutObject;

	m_pObject->SetState(true);



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
		if (index == 3)
			return 1;
		else
			return rng() % (count - 2) + 2;
	};

	auto addIngredient = [this, &layoutObject, &rng] (ObjectTransform tf, size_t id) mutable
	{
		auto& ingredient = IngredientEnity::Create(layoutObject, m_pData->ingrendients[id]);
		ingredient.AddComponent(std::move(tf));
		ingredient.GetComponent<IngredientEnity>().AddObserver(
			[this, state = true] (IngredientEnity::IsOnPlate) mutable
			{
				if (state)
				{
					state = false;
					++m_PlayerAttacks;
					Notify(MakeDataEvent<LevelDataEvent::ATTACKCOUNT>(m_PlayerAttacks));
					if (--m_IngredientsLeft == 0)
					{
						m_Timer = 2_s;
						m_PendingEvent = { LevelEvent::COMPLETED, m_ID };
					}
				}
			}
		).Detach();
		++m_IngredientsLeft;
	};

	using enum LevelLayoutData::TileType;

	std::vector<std::vector<tei::unit::Position>> ingredientPoints(layout.width / 4);

	std::vector<tei::unit::Position> enemyPoints{};
	tei::unit::Position playerPoint{};
	tei::unit::Position player2Point{};

	for (auto processedType : { LADDER, LADDER_PLATFORM, PLATFORM, LADDER_SHELF, SHELF })
	{
		for (int x{}; x < layout.width; ++x)
		{
			//int ingredientID{};
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
						ingredientPoints[x / 4].push_back(pos);
						//addIngredient({ pos }, getIngredientID(ingredientID++) );
						break;
					}
				}
			}
		}
	}

	for (auto& points : ingredientPoints)
	{
		std::ranges::sort(points, std::greater{}, [](auto p) { return p.y; });
		for (int ingrId{}; auto const& point : points)
			addIngredient({ point }, getIngredientID(ingrId++));
	}

	std::ranges::shuffle(enemyPoints, rng);

	for (int x{ 2 }; x < layout.width; x += 4)
	{
		add(StaticEntityData::PLATE, { Position{ tileRoot + tileDiff * Vec2{ x, layout.height + 2.5f } } });
	}

	// Enemies (tmp)
	{
		m_Players.clear();
		m_Players.push_back(Anima::Create(layoutObject, m_pData->anima[0], playerPoint).GetComponent<Anima>());

		auto pointIt{ enemyPoints.begin() };
		for ([[maybe_unused]] auto i : {1, 2, 3})
			Anima::Create(layoutObject, m_pData->anima[1], *pointIt++);

		for ([[maybe_unused]] auto i : {1, 2})
			Anima::Create(layoutObject, m_pData->anima[2], *pointIt++);
		
		if (m_pData->anima.size() > 4)
		for ([[maybe_unused]] auto i : {1})
			Anima::Create(layoutObject, m_pData->anima[3], *pointIt++);

		auto& last = Anima::Create(layoutObject, m_pData->anima.back(), m_pData->mode == "co-op" ? player2Point : *pointIt++);
		if (last.HasComponent<PlayerEffects>())
			m_Players.push_back(last.GetComponent<Anima>());
	}

	m_PlayerHealth = 5;
	m_PlayerAttacks = 8;

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
	if (!m_Timer.IsPending())
	{
		m_PendingEvent = {};
		m_Timer = 2_s;
		m_PlayerHealth = std::max<int>(m_PlayerHealth - 1, 0);
		Notify(MakeDataEvent<LevelDataEvent::HEALTH>(m_PlayerHealth));
		if (!allowed)
			m_PendingEvent = { LevelEvent::FAILED, m_ID };
		return allowed;
	}
	return allowed;
}

void Level::OnUpdate()
{
	if (m_Timer && m_PendingEvent.type != LevelEvent::NONE)
	{
		if (m_PendingEvent.type == LevelEvent::COMPLETED || m_PendingEvent.type == LevelEvent::FAILED)
		{
			m_pObject->SetState(false);
		}
		else
		{
			Notify(m_PendingEvent);
		}
	}
}

void Level::OnDisable()
{
	if (m_PendingEvent.type == LevelEvent::COMPLETED || m_PendingEvent.type == LevelEvent::FAILED)
		Notify(m_PendingEvent);
}

tei::ecs::Object& CreateGame(tei::ecs::Object& parent, std::string_view mode)
{
	//auto& gameObject = parent.AddChild();

	Resource<LevelData> levelData{ tei::Resources->LoadUnique<LevelData>("", mode) };
	auto& level = Level::Create(parent, levelData);

	// UI
	{
		IconbarDisplay::CreateLevelDisplay(level, levelData).AddComponent(ObjectTransform{ { unit::Position{ .65f, -.45f }, unit::Scale{ .05f } } });
		IconbarDisplay::CreateHealthDisplay(level, levelData).AddComponent(ObjectTransform{ { unit::Position{ -.65f, -.45f }, unit::Scale{ .05f } } });

		auto& score = ScoreDisplay::Create(level);
		score.AddComponent(ObjectTransform{ { unit::Position{ -.60f, .45f }, unit::Scale{ .05f } } });
		score.AddComponent<Observed<unit::Colour>>(unit::Colour{ 1, 0, 0, 1 });
		
		auto& attacks = PlayerAttackCount::Create(level);
		attacks.AddComponent(ObjectTransform{ { unit::Position{ .60f, .45f }, unit::Scale{ .05f } } });
		attacks.AddComponent<Observed<unit::Colour>>(unit::Colour{ 0, 1, 0, 1 });
	}

	level.AddComponent(std::move(levelData));

	auto& levelComp = level.GetComponent<Level>();
	levelComp.AddObserver(
		[&, levelID = int()](Level::LevelEvent const& event) mutable
		{
			if (event.type == event.COMPLETED && event.levelID == levelID)
				levelComp.CreateLayout(++levelID);
		}
	).Detach();

	levelComp.CreateLayout(0);

	return level;
}