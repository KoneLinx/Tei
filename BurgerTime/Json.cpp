#include "Json.h"

#include <fstream>

void Load(std::shared_ptr<nlohmann::json>& out, std::filesystem::path const& path)
{
	out = std::make_shared<nlohmann::json>();
	std::ifstream{ path } >> *out;
}

void LoadToError(std::string name, std::type_info const& type)
{
	throw std::runtime_error{ "While parsing json data: Failed to load \"" + std::string{ name } + "\" into type \"" + type.name() + '\"'};
}

void LoadToRequiredError(std::string const& name)
{
	throw std::runtime_error{ "While parsing json data: Could not resolve \"" + std::string{ name } + '\"'};
}
