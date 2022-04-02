#pragma once

#include "tei/application.h"
#include "tei/audio.h"
#include "tei/render.h"
#include "tei/resource.h"
#include "tei/scene.h"
#include "tei/time.h"
#include "tei/unit.h"
#include "tei/components.h"
#include "tei/input.h"
#include "tei/event.h"

namespace tei
{
	using namespace tei::external;
}

// Called after the engine has prepared internal resources and will start the game loop shortly
extern void TeiClientInit();

// Called after the engine has stopped the game loop and is cleaning up client data
extern void TeiClientCleanup();

namespace tei::common
{

	using namespace tei::literals;

	using tei::ecs::Object;
	using tei::Clock;

	using tei::Time;
	using tei::Application;
	using tei::Audio;
	using tei::Resources;

}