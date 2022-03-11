#include "TextureComponent.h"

#include <tei/render.h>
#include <tei/resource.h>

using tei::internal::render::Renderer;
using tei::internal::ecs::Object;
using tei::internal::components::ObjectTransform;
using tei::internal::components::TextureRenderComponent;
using tei::internal::components::SpriteRenderComponent;
using tei::internal::resource::Texture;
using tei::internal::resource::Resource;
using tei::internal::resource::Sprite;

void OnRender(TextureRenderComponent const& comp)
{
	auto& [texture, transform] = comp.refs;
	Renderer->DrawTexture(*texture, transform.world);
}

void OnRender(SpriteRenderComponent const& comp)
{
	auto& [sprite, transform] = comp.refs;
	Renderer->DrawSprite(*sprite, transform.world);
}