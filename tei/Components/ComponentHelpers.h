#pragma once

#include <tei/internal/ecs.h>

namespace tei::internal::components
{

	template <typename Component>
	Component* FindParentComponent(ecs::Object const& self)
	{
		ecs::Object const* parent{ &self };
		Component* comp{};
		do
		{
			parent = &parent->GetParent();
			comp = parent->HasComponent<Component>();
		} 
		while (comp == nullptr && !parent->IsRoot());
		return comp;
	}

}