#include "TextureComponent.h"

#include <tei/render.h>

using tei::internal::render::Renderer;
using tei::internal::ecs::Object;
using tei::internal::components::ObjectTransform;
using tei::internal::components::TextureRenderComponent;
using tei::internal::components::SpriteRenderComponent;
using tei::internal::resource::Texture;
using tei::internal::resource::Sprite;

void OnEnable(TextureRenderComponent& comp, Object const& obj)
{
	comp.pTexture = &obj.GetComponent<Texture>();
	comp.pTransform = &obj.GetComponent<ObjectTransform>();
}

void OnEnable(SpriteRenderComponent& comp, Object const& obj)
{
	comp.pSprite = &obj.GetComponent<Sprite>();
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