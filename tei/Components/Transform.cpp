#include "Transform.h"

using tei::internal::components::ObjectTransform;
using tei::internal::ecs::Object;

ObjectTransform const ObjectTransform::ROOT{};

void OnEnable(ObjectTransform& transform, Object const& object)
{
	Object const* parent{ &object };
	do
	{
		parent = &parent->GetParent();
		if (auto parentTransform{ parent->GetComponent<ObjectTransform>() })
		{
			transform.parent = parentTransform;
			break;
		}
	}
	while (!parent->IsRoot());
	OnUpdate(transform);
}

void OnUpdate(ObjectTransform& transform)
{
	//if (tranform.requiresUpdate)
	transform.world = transform.parent->world * transform.local;
}
