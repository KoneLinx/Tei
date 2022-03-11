
#include "Renderer.h"

#include <tei/application.h>
#include <tei/time.h>
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

	SDL_GetRendererOutputSize(m_SDLRenderer, &m_RenderTarget.w, &m_RenderTarget.h);

	tei::internal::time::Time->frame.vsynced = true;
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

	SDL_GetRendererOutputSize(m_SDLRenderer, &m_RenderTarget.w, &m_RenderTarget.h);
	m_TargetSize.x = float(m_RenderTarget.w);
	m_TargetSize.y = float(m_RenderTarget.h);
	m_TargetCenter = m_TargetSize / 2.f;
	m_TargetScale.x = std::min(m_TargetSize.x, m_TargetSize.y);
	m_TargetScale.y = m_TargetScale.x;
	m_TargetRatio = m_TargetSize / m_TargetScale;

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
	else
		tei::internal::time::Time->frame.vsynced = synced;
}

void RendererClass::DrawTexture(resource::Texture const& texture, unit::Transform const& transform, std::optional<unit::Rectangle> source) const
{
	METRICS_TIMEBLOCK;

	auto const sourceRatio{ source ? source.value()[1] : unit::Scale{ 1, 1 } };
	auto const texRatio{ sourceRatio * unit::Scale{ texture.w, texture.h } / float(std::min(texture.w, texture.h)) };
	auto const center = m_TargetCenter + transform.position / 2.f * m_TargetScale;
	auto const scale = transform.scale * m_TargetScale * texRatio / 2.f;
	auto const position = center - scale / 2.f;
	SDL_Rect const dest{
		.x = int(position.x),
		.y = int(position.y),
		.w = int(scale.x),
		.h = int(scale.y),
	};
	auto const src = [texture] (unit::Rectangle source) 
	{
		return SDL_Rect{
			.x = int(source[0].x * texture.w),
			.y = int(source[0].y * texture.h),
			.w = int(source[1].x * texture.w),
			.h = int(source[1].y * texture.h),
		}; 
	};
	SDL_RenderCopyEx(
		m_SDLRenderer,
		static_cast<SDL_Texture*>(texture.pData),
		source ? std::data({ src(*source) }) : nullptr,
		&dest,
		glm::degrees(transform.rotation.x),
		std::data({ SDL_Point{
			.x = int(scale.x / 2),
			.y = int(scale.y / 2),
		}}),
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
