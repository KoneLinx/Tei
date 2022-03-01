#include "ResourceManager.h"

#include <SDL.h>

namespace tei::internal::resource
{

	ResourceManager::Service Resources {};

	ResourceManager::ResourceManager(std::filesystem::path defaultPath)
		: m_DefaultPath{ defaultPath }
	{
		METRICS_TIMEBLOCK;

		std::filesystem::current_path(defaultPath);
	}

	ResourceManager::~ResourceManager()
	{}

	std::filesystem::path ResourceManager::GetExisting(std::filesystem::path const& path) const
	{
		if (std::filesystem::exists(path))
			return std::filesystem::absolute(path);

		else if (auto newpath = m_DefaultPath / path; std::filesystem::exists(newpath))
			return newpath;

		else throw utility::TeiRuntimeError{ "Could not find valid path", path.string() };
	}

}