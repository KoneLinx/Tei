#pragma once

#include <tei.h>

#include "../Level/Anima.h"
#include "../Level/StaticEntity.h"
#include "../Level/Ingredient.h"

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
	std::vector<StaticEntityData> other;
	std::vector<LevelLayoutData> levels;
};

class Level : public tei::components::Subject
{

public:

	static tei::ecs::Object& Create(tei::ecs::Object& parent, LevelData const& data);

	struct Event
	{
		enum struct Type
		{
			LOADED,
			PAUSED,
			RESUMED,
			COMPLETED,
			FAILED
		};
		using enum Type;

		Type type;
		int levelID;
	};

private:

	void CreateLayout(tei::ecs::Object& object, int id);

	int m_ID;

	LevelData const* m_pData;

};

