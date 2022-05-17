#pragma once

#include <tei.h>

#include "../Level/Anima.h"
#include "../Level/StaticEntity.h"
#include "../Level/Ingredient.h"

struct LevelData
{
	std::string_view mode;

	std::vector<AnimaData> anima;
	std::vector<IngredientData> ingrendients;
	std::vector<StaticEntityData> other;
};

class Level : public tei::components::Subject
{

public:

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

	int m_ID;

};

