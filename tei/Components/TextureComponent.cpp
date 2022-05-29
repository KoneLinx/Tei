#include "teipch.h"
#include "TextureComponent.h"

#include <tei/render.h>
#include <tei/resource.h>

void OnRender(std::nullptr_t, tei::internal::components::TextureRenderComponent const& comp)
{
	METRICS_TIMEBLOCK;

	auto& [texture, transform] = comp.Refs();
	tei::internal::render::Renderer->DrawTexture(*texture, transform.world);
}
	
void OnRender(std::nullptr_t, tei::internal::components::SpriteRenderComponent const& comp)
{
	METRICS_TIMEBLOCK;

	auto& [sprite, transform] = comp.Refs();
	tei::internal::render::Renderer->DrawSprite(*sprite, transform.world);
}