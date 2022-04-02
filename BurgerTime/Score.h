#pragma once

#include "Player.h"

struct Score : tei::components::Observed<long>
{};

struct ScoreDisplay : tei::components::RefComponent<tei::components::Observed<std::string>, Score>
{};

tei::ecs::Object& CreateScore(tei::ecs::Object&);

void OnEnable(Score&, tei::ecs::Object const&);

void OnUpdate(ScoreDisplay const&, tei::ecs::Object const&);