#pragma once

#include <tei/resource.h>
#include <tei/utility.h>
#include "RefComponent.h"
#include "Transform.h"

namespace tei::internal::components
{

	struct TextRenderComponent : RefComponent< 
		utility::Observed<std::string>,
		resource::Resource<resource::Font>,
		resource::Resource<resource::Texture>,
		components::ObjectTransform
	>
	{};

}

namespace tei::external
{
	namespace components
	{
		using tei::internal::components::TextRenderComponent;
		using tei::internal::utility::Observed;
	}
}

void OnUpdate(std::nullptr_t, tei::internal::components::TextRenderComponent const&);
void OnRender(std::nullptr_t, tei::internal::components::TextRenderComponent const&);
