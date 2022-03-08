#pragma once

#include "tei/application.h"
#include "tei/audio.h"
#include "tei/render.h"
#include "tei/resource.h"
#include "tei/scene.h"
#include "tei/time.h"
#include "tei/unit.h"
#include "tei/components.h"

namespace tei
{
	using namespace tei::external;
}

// Called after the engine has prepared internal resources and will start the game loop shortly
extern void TeiClientInit();

// Called after the engine has stopped the game loop and is cleaning up client data
extern void TeiClientCleanup();