#pragma once

#include <tei.h>

struct StaticEntityData
{
	using Type = std::string_view;

		static inline constexpr Type PLATE = "plate";
		static inline constexpr Type LADDER = "ladder";
		static inline constexpr Type PLATFORM = "platform";
		static inline constexpr Type SHELF = "shelf";
		static inline constexpr Type HEARTH = "heart";
		static inline constexpr Type LEVEL_1 = "level_1";
		static inline constexpr Type LEVEL_5 = "level_5";
		static inline constexpr Type LEVEL_10 = "level_10";

	Type type;

	tei::unit::Dimentions box;
	
	std::string name{};
	
	tei::Resource<tei::resource::Texture> texture;
};

class StaticEntity
{
};

