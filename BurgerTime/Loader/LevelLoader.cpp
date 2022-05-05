#include "pch.h"
#include "LevelLoader.h"

#include <array>
#include <string>
#include <fstream>
#include <set>
#include <nlohmann/json.hpp>

#include <tei.h>

using namespace std::literals;
using namespace tei;
using namespace tei::literals;

struct Anima
{

	struct Type
	{
		static inline constexpr auto PLAYER = "player"sv;
		static inline constexpr auto ENEMY  = "enemy"sv;
	};

	std::string_view type;
	unit::Dimentions box;

	std::string name{};
	
	struct State
	{
		enum struct ID : size_t
		{
			STATIONARY,
			WALKING_LEFT,
			WALKING_RIGHT,
			WALKING_DOWN,
			WALKING_UP,
			ATTACKING,
			DYING,
			HIT
		} 
		state{};
		using enum ID;
		std::vector<Resource<resource::Sprite>> sprites{};
	}
	state{};

	long score{};
	bool flips{};

};

struct Ingredient
{
	std::string name;

	Resource<resource::Texture> texture;

	bool unique{};
	int position{};
};

struct Entity
{
	struct Type
	{
		static inline constexpr auto PLATE = "plate"sv;
		static inline constexpr auto LADDER = "ladder"sv;
		static inline constexpr auto PLATFORM = "platform"sv;
		static inline constexpr auto SHELF = "shelf"sv;
	};

	std::string_view type;
	unit::Dimentions box;
	
	std::string name{};
	
	Resource<resource::Texture> texture;
};

struct Level
{
	std::string_view mode;

	std::vector<Anima> anima;
	std::vector<Ingredient> ingrendients;
	std::vector<Entity> other;
};

constexpr inline auto resourcedir = "resources/"sv;

bool resembles(nlohmann::json const& data, std::string_view word)
{
	// Null always true
	if (data.is_null())
		return true;

	// String compare
	if (data.is_string())
		return std::string_view{ data } == word;

	// Set includes
	if (data.is_array())
	{

		for (auto& entry : data)
		{
			if (entry.is_string() && std::string_view{ entry } == word)
				return true;
		}
	}
	return false;
}

std::vector<Anima> LoadAnima(Level const& level, nlohmann::json const& entitydata)
{

	auto& animadata = entitydata["anima"];

	std::vector<Anima> anima{};
	std::vector<Anima::State> states{};

	unit::Dimentions box{};
	box.x = animadata["box"][0];
	box.y = animadata["box"][1];

	std::string_view spritesep{ entitydata["spriteseparator"] };
	std::string_view spriteext{ entitydata["spriteextention"] };
	Clock::duration spritedur{ 1_s / long(entitydata["spritefrequency"]) };

	for (auto& entitydata : animadata["entities"])
	{
		if (resembles(entitydata["mode"], level.mode))
		{
			Anima entity{
				.type = entitydata["type"],
				.box = box,
				.name = entitydata["name"]
			};
			if (resembles(entitydata["type"], Anima::Type::ENEMY))
			{
				entity.score = entitydata["score"];
				entity.flips = entitydata["flips"];
			}
			
			std::string_view animasrc = entitydata["src"];
			entity.state.sprites.reserve(5);

			for (auto& statedata : animadata["states"])
			{
				if (resembles(statedata["type"], entity.type))
				{
					if (statedata.find("states") != statedata.end())
					{
						for (std::string_view substate : statedata["states"])
							entity.state.sprites.push_back(
								Resources->LoadShared<resource::Sprite>(
									std::string{ resourcedir }.append(animasrc).append(spritesep).append<std::string_view>(statedata["src"]).append(spritesep).append<std::string_view>(substate).append(spriteext),
									spritedur, statedata["frames"], 1, statedata["loop"]
									)
							);
					}
					else
					{
						entity.state.sprites.push_back(
							Resources->LoadShared<resource::Sprite>(
								std::string{ resourcedir }.append(animasrc).append(spritesep).append<std::string_view>(statedata["src"]).append(spriteext),
								spritedur, statedata["frames"], 1, statedata["loop"]
								)
						);
					}
				}
				else entity.state.sprites.push_back({});
			}

			anima.push_back(std::move(entity));
		}
	}

	return anima;
}

std::vector<Ingredient> LoadIngredients(nlohmann::json const& entitydata)
{
	auto const& ingredientsdata = entitydata["ingredients"];
	
	std::vector<Ingredient> ingredients{};
	ingredients.reserve(6);

	std::string_view spritesep{ entitydata["spriteseparator"] };
	std::string_view spriteext{ entitydata["spriteextention"] };
	
	unit::Dimentions box{};
	box.x = ingredientsdata["box"][0];
	box.y = ingredientsdata["box"][1];

	for (auto& entitydata : ingredientsdata["entities"])
	{
		Ingredient ingredient{
			.name = entitydata["name"],
			.texture = Resources->LoadShared<resource::Texture>(
				std::string{ resourcedir }.append<std::string_view>(entitydata["src"]).append(spriteext)
			)
		};
		if (entitydata["unique"].is_boolean() && bool(entitydata["unique"]))
		{
			ingredient.unique = true;
			ingredient.position = entitydata["position"].is_number() ? int(entitydata["position"]) : int();
		}
		ingredients.push_back(std::move(ingredient));
	}

	return ingredients;
}

std::vector<Entity> LoadOther(nlohmann::json const& entitydata)
{
	auto const& otherdata = entitydata["other"];
	
	std::vector<Entity> other{};
	other.reserve(4);
	
	std::string_view spritesep{ entitydata["spriteseparator"] };
	std::string_view spriteext{ entitydata["spriteextention"] };

	std::set const types{
		Entity::Type::PLATE,
		Entity::Type::LADDER,
		Entity::Type::PLATFORM,
		Entity::Type::SHELF
	};

	for (auto& entitydata : otherdata["entities"].items())
	{
		
		unit::Dimentions box{};
		box.x = entitydata.value()["box"][0];
		box.y = entitydata.value()["box"][1];

		other.push_back(Entity{
			.type = *types.find(entitydata.key()),
			.box = box,
			.name = entitydata.value()["name"],
			.texture = Resources->LoadShared<resource::Texture>(
				std::string{ resourcedir }.append<std::string_view>(entitydata.value()["src"]).append(spriteext)
			)
		});
	}

	return other;
}

Level LoadLevel([[maybe_unused]] nlohmann::json const& leveldata)
{
	try
	{
		Level level{ .mode = "Single player" };

		nlohmann::json const entitydata =
		[] (std::filesystem::path const& path)
		{
			nlohmann::json data{};
			std::ifstream{ path } >> data;
			return data;
		} 
		("resources/EntityData.json");

		level.anima = LoadAnima(level, entitydata);
		level.ingrendients = LoadIngredients(entitydata);
		level.other = LoadOther(entitydata);

		return level;
	}
	catch (const std::exception& e)
	{
		puts("[ERROR] An error occured while loading level data from datafile");
		puts(e.what());
		throw;
	}
}

void test()
{
	LoadLevel({});
}
