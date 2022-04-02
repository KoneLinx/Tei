#pragma once

#include <tei/resource.h>
#include "RefComponent.h"
#include "Transform.h"

namespace tei::internal::components
{

	struct TextureRenderComponent : RefComponent<resource::Resource<resource::Texture>, components::ObjectTransform>
	{};

	struct SpriteRenderComponent : RefComponent<resource::Resource<resource::Sprite>, components::ObjectTransform>
	{};

}

namespace tei::external::components
{
	using tei::internal::components::TextureRenderComponent;
	using tei::internal::components::SpriteRenderComponent;
}

void OnRender(std::nullptr_t, tei::internal::components::TextureRenderComponent const&);
void OnRender(std::nullptr_t, tei::internal::components::SpriteRenderComponent const&);
