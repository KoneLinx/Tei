#include "TextureComponent.h"

#include <tei/render.h>
#include <tei/resource.h>

void OnRender(tei::internal::Internal, tei::internal::components::TextureRenderComponent const& comp)
{
	auto& [texture, transform] = comp.refs;
	tei::internal::render::Renderer->DrawTexture(*texture, transform.world);
}
	
void OnRender(tei::internal::Internal, tei::internal::components::SpriteRenderComponent const& comp)
{
	auto& [sprite, transform] = comp.refs;
	tei::internal::render::Renderer->DrawSprite(*sprite, transform.world);
}