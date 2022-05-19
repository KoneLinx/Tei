#pragma once

#include "../Level/Level.h"

LevelData LoadLevelData(std::filesystem::path const& path, std::string_view mode);

void Load(std::shared_ptr<LevelData>& ref, std::filesystem::path const& path, std::string_view mode);