#include "teipch.h"

#include "Renderer.h"

#include <tei/application.h>
#include <SDL.h>

#include <ImGui.h>

namespace tei::internal::render
{

	RendererClass::Service Renderer {};

}

using namespace tei::internal::render;
using namespace tei::internal::resource;
using namespace tei::internal::utility;
using namespace tei::internal;

#define m_SDLRenderer RefAs<SDL_Renderer>(m_RenderTarget.pData)

RendererClass::RendererClass(application::Application::Window const& window)
{
	METRICS_TIMEBLOCK;

	m_SDLRenderer = SDL_CreateRenderer(
		static_cast<SDL_Window*>(window.pData),
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (m_SDLRenderer == nullptr)
		throw utility::TeiRuntimeError{ "Could not load SDL Renerer", SDL_GetError() };

	Update();
}

RendererClass::~RendererClass()
{
	METRICS_TIMEBLOCK;
	if (m_SDLRenderer != nullptr)
		SDL_DestroyRenderer(m_SDLRenderer);
}

void RendererClass::Clear()
{
	METRICS_TIMEBLOCK;
	SDL_RenderClear(m_SDLRenderer);

	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void RendererClass::Update()
{
	METRICS_TIMEBLOCK;

	int w{}, h{};
	SDL_GetRendererOutputSize(m_SDLRenderer, &w, &h);
	m_RenderTarget.size = { w, h };
	m_TargetCenter = m_RenderTarget.size / 2.f;
	//m_TargetScale.x = std::min(m_RenderTarget.size.x, m_RenderTarget.size.y);
	//m_TargetScale.y = m_TargetScale.x;
	//m_TargetRatio = m_RenderTarget.size / m_TargetScale;

	m_Time = time::Time->frame.now;
}

void RendererClass::Present()
{
	METRICS_TIMEBLOCK;

	ImGui::Render();
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

	SDL_RenderPresent(m_SDLRenderer);
}

void RendererClass::SetVSync(bool synced) const
{
	METRICS_TIMEBLOCK;
	if (SDL_RenderSetVSync(m_SDLRenderer, int(synced)) != 0)
		throw utility::TeiRuntimeError{ "Could not alter vsync status", !synced };
}

void RendererClass::DrawTexture(resource::Texture const& texture, unit::Transform const& transform, std::optional<unit::Rectangle> source) const
{
	if (texture.pData == nullptr)
		return;

	auto const scale = texture.size * transform.scale;// * (m_TargetScale / 480.f);
	auto const pos   = m_TargetCenter + transform.position - scale / 2.f;
	auto const angle = glm::degrees(transform.rotation.r);

	constexpr static auto src = [](resource::Texture const& texture, unit::Rectangle const& rect) -> SDL_Rect
	{
		return {
			.x = int(rect[0].x * texture.size.x),
			.y = int(rect[0].y * texture.size.y),
			.w = int(rect[1].x * texture.size.x),
			.h = int(rect[1].y * texture.size.y),
		};
	};

	SDL_Rect dest{
		.x = int(pos.x),
		.y = int(pos.y),
		.w = int(scale.x),
		.h = int(scale.y),
	};

	SDL_Point pivot{
		.x = int(scale.x) / 2,
		.y = int(scale.y) / 2,
	};


	SDL_RenderCopyEx(
		m_SDLRenderer,
		static_cast<SDL_Texture*>(texture.pData),
		source ? std::data({ src(texture, *source) }) : nullptr,
		&dest,
		angle,
		&pivot,
		SDL_FLIP_NONE
	);
}

void RendererClass::DrawSprite(resource::Sprite const& sprite, unit::Transform const& transform) const
{
	METRICS_TIMEBLOCK;

	auto frame = unsigned((m_Time - sprite.origintime) / sprite.frameduration);

	if (sprite.loop == false)
		frame = std::min<unsigned>(frame, sprite.rows * sprite.cols - 1);

	unit::Vec2 const div{
		1.f / sprite.cols,
		1.f / sprite.rows
	};
	unit::Vec2 const pos{
		frame % sprite.cols,
		frame / sprite.cols % sprite.rows
	};

	DrawTexture(sprite, transform, unit::Rectangle{ div * pos, div });
}
