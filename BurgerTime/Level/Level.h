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
			LOADED,
			COMPLETED,
			FAILED
		};
		using enum Type;

		Type type;
		int levelID;
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

private:

	int m_ID{};

	tei::ecs::Object* m_pObject{};

	LevelData const* m_pData{};

	int m_PlayerHealth{};
	int m_PlayerAttacks{};

};

