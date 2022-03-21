#pragma once

#include <tei/resource.h>
#include <tei/internal/ecs.h>
#include "RefComponent.h"
#include "Transform.h"

namespace tei::internal::components
{

	using TextureRenderComponent = RefComponent<"trc", resource::Resource<resource::Texture>, components::ObjectTransform>;

	using SpriteRenderComponent = RefComponent<"src", resource::Resource<resource::Sprite>, components::ObjectTransform>;

}

namespace tei::external::components
{
	using tei::internal::components::TextureRenderComponent;
	using tei::internal::components::SpriteRenderComponent;
}

void OnRender(tei::internal::Internal, tei::internal::components::TextureRenderComponent const&);
void OnRender(tei::internal::Internal, tei::internal::components::SpriteRenderComponent const&);
