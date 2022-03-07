#pragma once

#include <tei.h>

void OnInitialize(std::string&, tei::ecs::Object&);

void OnEnable(std::string_view);

void OnUpdate(std::string_view);

void OnRender(std::string_view);

void OnDisable(std::string_view);

void OnCleanup(std::string_view);
