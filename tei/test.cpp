
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <ImGui.h>

#include <iostream>
#include <thread>

#include <tei/time.h>

using namespace tei::internal::time;
using namespace tei::internal::time::literals;

void t1(std::chrono::steady_clock::time_point) {}
void t2(std::chrono::high_resolution_clock::time_point) {}

int main(int argc, char*[])
{
	if (argc == 0)
	// Always false, build and link test only
	{
		(void)SDL_getenv(nullptr);
		(void)TTF_CloseFont(nullptr);
		(void)IMG_LoadTGA_RW(nullptr);
		(void)ImGui_ImplSDL2_InitForSDLRenderer(nullptr, nullptr);
		(void)ImGui::Render();
	}

	auto now = Clock::now();

	std::cout << std::chrono::system_clock::now();
	std::this_thread::sleep_until(now + 3_s);

	std::cout << std::chrono::system_clock::now();
	std::this_thread::sleep_for(3_s);

	std::cout << std::chrono::system_clock::now();
	return 0;
}