#pragma once

#include "../Level/Level.h"

LevelData LoadLevelData(std::filesystem::path const& path);

void Load(std::shared_ptr<LevelData>& ref, std::filesystem::path const& path);