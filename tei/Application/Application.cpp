#include "Application.h"

#include <tei/render.h>
#include <tei/internal/core.h>
#include <tei/time.h>
#include <SDL.h>

#include <ImGui.h>

#include "../Extra/StaticEngineResources.h"

namespace tei::internal::application
{

	Application::Service ApplicationService{};

}

using namespace tei::internal::application;
using namespace tei::internal;
using namespace std::literals;

#define m_SDLWindow utility::RefAs<SDL_Window>(m_Window.pData)

Application::Application(int argc, char const* const* argv)
	: m_Args{ 
		argv,
		argv + (argv != nullptr ? argc : 0) 
	}
{
	METRICS_TIMEBLOCK;

	constexpr static auto subSystems{
		SDL_INIT_VIDEO |
		SDL_INIT_AUDIO
	};

	if (SDL_Init(subSystems) != 0)
		throw std::runtime_error{ "Could nit initialize SDL: "s + SDL_GetError() };

	OpenWindow();
}

Application::~Application()
{
	METRICS_TIMEBLOCK;
	render::Renderer.Register(nullptr);
	SDL_Quit();
}

void Application::SetFullscreen(bool state, bool fake)
{
	METRICS_TIMEBLOCK;
	SDL_SetWindowFullscreen(m_SDLWindow, state ? (fake ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_FULLSCREEN) : 0);
	SDL_GetWindowSize(m_SDLWindow, &m_Window.w, &m_Window.h);
	SDL_GetWindowPosition(m_SDLWindow, &m_Window.x, &m_Window.y);
}

void Application::SetWindowSize(int width, int height)
{
	auto old{ m_Window };
	SDL_SetWindowSize(m_SDLWindow, width, height);
	SDL_GetWindowSize(m_SDLWindow, &m_Window.w, &m_Window.h);
	SDL_SetWindowPosition(m_SDLWindow, old.x + (old.w - m_Window.w) / 2, old.y + (old.h - m_Window.h) / 2);
	SDL_GetWindowPosition(m_SDLWindow, &m_Window.x, &m_Window.y);
}

void Application::SetWindowBorder(bool state)
{
	SDL_SetWindowBordered(m_SDLWindow, state ? SDL_TRUE : SDL_FALSE);
	SDL_SetWindowResizable(m_SDLWindow, SDL_TRUE);
}

void Application::Quit() const
{
	METRICS_TIMEBLOCK;
	tei::internal::core::Core->Stop();
}

void Application::OpenWindow()
{
	SDL_Rect display{};
	if (SDL_GetDisplayBounds(0, &display) == 0)
		display.w = (display.h = 1080) * 18 / 9;

	METRICS_TIMEBLOCK;
	m_SDLWindow = SDL_CreateWindow(
		"Tei engine",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		display.w / 3,
		display.h / 3,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS
	);

	if (m_SDLWindow == nullptr)
		throw utility::TeiRuntimeError{ "Could not create SDL Window", SDL_GetError() };

	SDL_SetWindowBordered(m_SDLWindow, SDL_FALSE);

	SDL_GetWindowSize(m_SDLWindow, &m_Window.w, &m_Window.h);
	SDL_GetWindowPosition(m_SDLWindow, &m_Window.x, &m_Window.y);

	auto& renderer{ render::Renderer.Register(new render::RendererClass{ m_Window }) };

	auto target = static_cast<SDL_Renderer*>(renderer.GetRenderTraget().pData);
	ImGui::CreateContext();
	ImGui_ImplSDLRenderer_Init(target);
	ImGui_ImplSDL2_InitForSDLRenderer(m_SDLWindow, target);

	renderer.Update();
	renderer.Clear();
	renderer.DrawTexture(extra::resources::EngineLoaderBackground(), unit::Scale{2, 2});
	renderer.Present();

}

void Application::CloseWindow()
{
	METRICS_TIMEBLOCK;

	ImGui_ImplSDL2_Shutdown();
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui::DestroyContext();

	render::Renderer.Register(nullptr);

	if (m_SDLWindow != nullptr)
		SDL_DestroyWindow(std::exchange(m_SDLWindow, nullptr));
}

void Application::InitAudio()
{
}

void Application::ExitAudio()
{
}

Application& tei::internal::application::StartApplication(int argc, char const* const* argv)
{
	METRICS_TIMEBLOCK;
	return ApplicationService.Register(new Application{ argc, argv });
}

void tei::internal::application::StopApplication()
{
	METRICS_TIMEBLOCK;
	ApplicationService.Register(nullptr);
}
