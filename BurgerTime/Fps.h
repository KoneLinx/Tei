#pragma once

#include <tei.h>

struct FpsComponent : tei::components::RefComponent<"fps", tei::components::Observed<std::string>, tei::components::ObjectTransform>
{
	tei::Clock::duration average{ 1 };
};

tei::ecs::Object& CreateFps(tei::ecs::Object&);

void OnEnable(FpsComponent& fps, tei::ecs::Object& object);

void OnUpdate(FpsComponent&);

