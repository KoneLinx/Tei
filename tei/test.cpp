
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <ImGui.h>

#include <iostream>
#include <thread>

#include <tei/time.h>
#include <tei/unit.h>
#include <tei/application.h>

using namespace tei::internal::time;
using namespace tei::internal::unit;
using namespace tei::internal::time::literals;

void t1(std::chrono::steady_clock::time_point) {}
void t2(std::chrono::high_resolution_clock::time_point) {}

int main(int argc, char* argv[])
{
	METRICS_INITLOG("_log.json");
	METRICS_TIMEBLOCK;

	tei::internal::application::StartApplication(argc, argv);

	(void)tei::external::Application->Args()[0];

	return 0;
}
