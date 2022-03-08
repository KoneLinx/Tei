#include "Application.h"

#include <tei/render.h>
#include <tei/internal/core.h>
#include <tei/time.h>
#include <SDL.h>

namespace tei::internal::application
{

	Application::Service ApplicationService{};

}

using namespace tei::internal::application;
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

void Application::Quit() const
{
	METRICS_TIMEBLOCK;
	tei::internal::core::Core->Stop();
}

void Application::OpenWindow()
{
	METRICS_TIMEBLOCK;
	m_SDLWindow = SDL_CreateWindow(
		"Tei engine",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640,
		480,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);

	if (m_SDLWindow == nullptr)
		throw utility::TeiRuntimeError{ "Could not create SDL Window", SDL_GetError() };

	SDL_GetWindowSize(m_SDLWindow, &m_Window.w, &m_Window.h);
	SDL_GetWindowPosition(m_SDLWindow, &m_Window.x, &m_Window.y);

	render::Renderer.Register(new render::RendererClass{ m_Window });
}

void Application::CloseWindow()
{
	METRICS_TIMEBLOCK;
	if (m_SDLWindow != nullptr) SDL_DestroyWindow(std::exchange(m_SDLWindow, nullptr));
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
