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

void OnEnable(TextureRenderComponent& comp, Object const& obj)
{
	comp.pTexture = std::to_address(obj.GetComponent<Resource<Texture>>());
	comp.pTransform = &obj.GetComponent<ObjectTransform>();
}

void OnEnable(SpriteRenderComponent& comp, Object const& obj)
{
	comp.pSprite = std::to_address(obj.GetComponent<Resource<Sprite>>());
	comp.pTransform = &obj.GetComponent<ObjectTransform>();
}

void OnRender(TextureRenderComponent const& comp)
{
	Renderer->DrawTexture(*comp.pTexture, comp.pTransform->world);
}

void OnRender(SpriteRenderComponent const& comp)
{
	Renderer->DrawSprite(*comp.pSprite, comp.pTransform->world);
}