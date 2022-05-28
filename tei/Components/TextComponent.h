#pragma once

#include <tei/resource.h>
#include <tei/utility.h>
#include "RefComponent.h"
#include "Transform.h"

namespace tei::internal::components
{

	struct TextRenderComponent : public RefComponent< 
		utility::Observed<std::string>,
		resource::Resource<resource::Font>,
		resource::Resource<resource::Texture>,
		utility::Observed<unit::Colour>,
		components::ObjectTransform
	>
	{
		inline utility::Observed<std::string>& Text()
		{
			return Refs();
		}
		inline utility::Observed<unit::Colour>& Colour()
		{
			return Refs();
		}
	};

}

namespace tei::external
{
	namespace components
	{
		using tei::internal::components::TextRenderComponent;
		using tei::internal::utility::Observed;
	}
}

void OnInitialize(std::nullptr_t, tei::internal::components::TextRenderComponent const&, tei::internal::ecs::Object& object);
void OnUpdate(std::nullptr_t, tei::internal::components::TextRenderComponent const&);
void OnRender(std::nullptr_t, tei::internal::components::TextRenderComponent const&);
