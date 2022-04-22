#include "Application.h"

#include <iostream>

#include <tei/render.h>
#include <tei/internal/core.h>
#include <tei/internal/Utility/Error.h>
#include <tei/internal/Utility/TypeHelpers.h>
#include <tei/time.h>
#include <tei/event.h>

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <ImGui.h>

#include "../Steam.h"

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
	: m_Args{ argv, argv + (argv != nullptr ? argc : 0) }
	, m_Window{}
{
	METRICS_TIMEBLOCK;

	constexpr static auto subSystems{
		SDL_INIT_VIDEO |
		SDL_INIT_AUDIO
	};

	if (SDL_Init(subSystems) != 0)
		throw std::runtime_error{ "Could not initialize SDL: "s + SDL_GetError() };

	if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
		throw utility::TeiRuntimeError{ "Could not initialize SDL_image", SDL_GetError() };

	OpenWindow();

	if (!SteamInitialize())
		0;//utility::TeiRuntimeError{ "Could not initialize Steam API" };
}

Application::~Application()
{
	METRICS_TIMEBLOCK;

	SteamCleanup();

	if (m_SDLWindow)
		CloseWindow();

	SDL_Quit();
}

void Application::UpdateProps()
{
	int x{}, y{};
	SDL_GetWindowSize(m_SDLWindow, &x, &y);
	m_Window.transform.scale.x = float(x), m_Window.transform.scale.y = float(y);
	SDL_GetWindowPosition(m_SDLWindow, &x, &y);
	m_Window.transform.position.x = float(x), m_Window.transform.position.y = float(y);
	
	if (render::Renderer)
		render::Renderer->Update();
	
	if (events::Event)
		events::Event->Notify<WindowPropertyChangedEvent>();
}

void Application::SetWindowProperty(unit::Scale size)
{
	auto old{ m_Window.transform };
	int x{}, y{};
	SDL_SetWindowSize(m_SDLWindow, int(size.x), int(size.y));
	SDL_GetWindowSize(m_SDLWindow, &x, &y);
	m_Window.transform.scale.x = float(x), m_Window.transform.scale.y = float(y);
	SDL_SetWindowPosition(m_SDLWindow, int(old.position.x + (old.scale.x - m_Window.transform.scale.x) / 2), int(old.position.y + (old.scale.y - m_Window.transform.scale.y) / 2));
	UpdateProps();
}

void Application::SetWindowProperty(unit::Position pos)
{
	SDL_SetWindowPosition(m_SDLWindow, int(pos.x), int(pos.y));
	UpdateProps();
}

void Application::SetWindowProperty(unit::Transform transform)
{
	SDL_SetWindowSize(m_SDLWindow, int(transform.scale.x), int(transform.scale.y));
	SDL_SetWindowPosition(m_SDLWindow, int(transform.position.x), int(transform.position.y));
	UpdateProps();
}

void Application::SetWindowProperty(WindowProperty property)
{
	switch (property)
	{
	case tei::internal::application::WindowProperty::BORDERED:
		SDL_SetWindowBordered(m_SDLWindow, SDL_TRUE);
		break;
	case tei::internal::application::WindowProperty::UNBORDERED:
		SDL_SetWindowBordered(m_SDLWindow, SDL_FALSE);
		break;
	case tei::internal::application::WindowProperty::RESIZABLE:
		SDL_SetWindowResizable(m_SDLWindow, SDL_TRUE);
		break;
	case tei::internal::application::WindowProperty::FIXED:
		SDL_SetWindowResizable(m_SDLWindow, SDL_FALSE);
		break;
	case tei::internal::application::WindowProperty::FULLSCREEN:
		SDL_SetWindowFullscreen(m_SDLWindow, SDL_WINDOW_FULLSCREEN);
		break;
	case tei::internal::application::WindowProperty::FULLSCREEN_FAKE:
		SDL_SetWindowFullscreen(m_SDLWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
		break;
	case tei::internal::application::WindowProperty::MAXIMISED:
		SDL_MaximizeWindow(m_SDLWindow);
		break;
	case tei::internal::application::WindowProperty::MINIMISED:
		SDL_MinimizeWindow(m_SDLWindow);
		break;
	case tei::internal::application::WindowProperty::RESTORED:
		SDL_RestoreWindow(m_SDLWindow);
		SDL_SetWindowAlwaysOnTop(m_SDLWindow, SDL_FALSE);
		break;
	case tei::internal::application::WindowProperty::ALWAYS_IN_FRONT:
		SDL_SetWindowAlwaysOnTop(m_SDLWindow, SDL_TRUE);
		break;
	}
	
	UpdateProps();
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

	SetWindowProperty(WindowProperty::UNBORDERED);

	auto& renderer{ render::Renderer.Register(new render::RendererClass{ m_Window }) };

	auto target = static_cast<SDL_Renderer*>(renderer.GetRenderTraget().pData);
	ImGui::CreateContext();
	ImGui_ImplSDLRenderer_Init(target);
	ImGui_ImplSDL2_InitForSDLRenderer(m_SDLWindow, target);

	auto splash = extra::resources::EngineLoaderBackground();

	renderer.Update();
	renderer.Clear();
	renderer.DrawTexture(splash, unit::Scale{renderer.GetRenderTraget().size.y / splash.size.y});
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
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
		throw utility::TeiRuntimeError{ "Could not open audio", SDL_GetError() };
}

void Application::ExitAudio()
{
	Mix_CloseAudio();
}

void Application::Update()
{
	for (SDL_Event event{}; SDL_PollEvent(&event);)
	{
		if (event.type == SDL_QUIT)
			Quit();
		else
			events::Event->Notify(event);
	}
	SteamUpdate();
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
