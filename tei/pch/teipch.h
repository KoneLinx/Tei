#pragma once

#include <ranges>
#include <string>
#include <vector>
#include <filesystem>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace fs = std::filesystem;
namespace stdr = std::ranges;
namespace stdv = std::views;

#include <tei/internal/utility.h>
#include <tei/internal/ecs.h>
#include <tei/time.h>
#include <tei/unit.h>

using namespace tei::internal;

#if true // All std

#include <thread>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <chrono>

#endif
#if false // All tei

#include <tei.h>
#include <tei/internal/core.h>

#endif
#if true // Dependencies

#include <ImGui.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

// glm.hpp included by tei/unit.h

#endif