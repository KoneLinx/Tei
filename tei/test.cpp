
#define METRICS

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <ImGui.h>

#include <iostream>
#include <thread>

#include <tei/time.h>
#include <tei/unit.h>
#include <tei/application.h>
#include <tei/render.h>
#include <tei/audio.h>
#include <tei/ecs.h>

using enum tei::internal::ecs::Message;
using namespace tei::internal::ecs;

int g_Int{};

template <>
void On<INIT>(float& f)
{
	METRICS_TIMEBLOCK;
	g_Int + f;
}

template <>
void On<FREE>(float& f)
{
	METRICS_TIMEBLOCK;
	g_Int + f;
}

template <>
void On<ENABLE>(float& f)
{
	METRICS_TIMEBLOCK;
	g_Int + f;
}
template <>
void On<DISABLE>(float& f)
{
	METRICS_TIMEBLOCK;
	g_Int + f;
}
template <>
void On<UPDATE>(float& f)
{
	METRICS_TIMEBLOCK;
	g_Int + f;
}
template <>
void On<RENDER>(float& f)
{
	METRICS_TIMEBLOCK;
	g_Int + f;
}

int main(int, char*[])
{
	METRICS_INITLOG("_log.json");
	METRICS_TIMEBLOCK;

	Object object{ true };

	auto& c1 = object.AddChild();

	c1.AddComponent(3.f);
	c1.AddComponent(5.f);

	auto& c2 = c1.AddChild(false);
	c2.AddComponent(7.f);

	auto& c3 = object.AddChild();
	c3.AddComponent(11.f);

	object.Do(ENABLE);

	object.Do(UPDATE);
	object.Do(RENDER);

	object.Do(UPDATE);
	object.Do(RENDER);

	object.Do(UPDATE);
	object.Do(RENDER);

	object.Do(UPDATE);
	object.Do(RENDER);

	c2.Activate(true);
	c3.Activate(false);

	object.Do(UPDATE);
	object.Do(RENDER);

	object.Do(UPDATE);
	object.Do(RENDER);

	object.Do(UPDATE);
	object.Do(RENDER);

	object.Do(UPDATE);
	object.Do(RENDER);

	object.Do(FREE);

	return 0;
}
