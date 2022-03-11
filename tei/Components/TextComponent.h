#pragma once

#include <tei/resource.h>
#include "RefComponent.h"
#include "Transform.h"

namespace tei::internal::components
{

	using TextRenderComponent = RefComponent<"trc", utility::Observed<std::string>, resource::Resource<resource::Font>, resource::Resource<resource::Texture>, components::ObjectTransform>;

}

namespace tei::external
{
	namespace components
	{
		using tei::internal::components::TextRenderComponent;
		using tei::internal::utility::Observed;
	}
}

void OnUpdate(tei::internal::components::TextRenderComponent const&);
void OnRender(tei::internal::components::TextRenderComponent const&);
