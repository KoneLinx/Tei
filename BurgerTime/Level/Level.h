#pragma once

#include <tei.h>

#include <map>

#include "Anima.h"
#include "StaticEntity.h"
#include "Ingredient.h"
#include "Particle.h"

struct LevelLayoutData
{
	enum struct TileType : char
	{
		NONE = ' ',
		PLATFORM = '_',
		LADDER = '=',
		SHELF = 's',
		LADDER_SHELF = 'n',
		LADDER_PLATFORM = 'b'
	};

	std::vector<TileType> field;
	int width, height;
};

struct LevelData
{
	std::string_view mode;

	std::vector<AnimaData> anima;
	std::vector<IngredientData> ingrendients;
	std::map<StaticEntityData::Type, StaticEntityData> statics;
	std::map<ParticleData::Type, ParticleData> particles;
	std::vector<LevelLayoutData> levels;
};

class Level : public tei::components::Subject
{

public:

	static tei::ecs::Object& Create(tei::ecs::Object& parent, LevelData const& data);

	struct LevelEvent
	{
		enum struct Type
		{
			NONE,
			LOADED,
			COMPLETED,
			ALL_COMPLETED,
			FAILED
		};
		using enum Type;

		Type type;
		int levelID{};
	};

	struct LevelDataEvent
	{
		enum Type : unsigned
		{
			HEALTH,
			ENEMYCOUNT,
			ATTACKCOUNT
		};
		
		std::variant<
			int, // health
			int, // enemycount
			int // attackcount
		> data;
	};

	tei::ecs::Object& CreateLayout(int id);

	bool DoPlayerAttack();
	bool DoPlayerDeath();

	std::span<std::reference_wrapper<Anima const> const> GetPlayers() const
	{ return m_Players; }

	void OnUpdate();
	void OnDisable();

private:

	int m_ID{};

	tei::ecs::Object* m_pObject{};
	tei::ecs::Object* m_pLayout{};

	LevelData const* m_pData{};

	int m_PlayerHealth{};
	int m_PlayerAttacks{};
	int m_IngredientsLeft{};

	std::vector<std::reference_wrapper<Anima const>> m_Players{};

	tei::time::TimeOnce m_Timer{};
	LevelEvent m_PendingEvent{ LevelEvent::NONE };

};

tei::ecs::Object& CreateGame(tei::ecs::Object& parent, std::string_view mode);