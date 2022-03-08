#pragma once

#include <tei/resource.h>
#include "Transform.h"

namespace tei::internal::components
{

	struct TextureRenderComponent
	{
		resource::Texture const* pTexture;
		components::ObjectTransform const* pTransform;
	};

	struct SpriteRenderComponent
	{
		resource::Sprite const* pSprite;
		components::ObjectTransform const* pTransform;
	};

}

namespace tei::external::components
{
	using tei::internal::components::TextureRenderComponent;
	using tei::internal::components::SpriteRenderComponent;
}

void OnEnable(tei::internal::components::TextureRenderComponent&, tei::internal::ecs::Object const&);
void OnEnable(tei::internal::components::SpriteRenderComponent&, tei::internal::ecs::Object const&);

//void OnUpdate(tei::internal::render::detail::SpriteRenderComponent&);

void OnRender(tei::internal::components::TextureRenderComponent const&);
void OnRender(tei::internal::components::SpriteRenderComponent const&);

