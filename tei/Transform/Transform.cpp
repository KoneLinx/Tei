#include "Transform.h"

using tei::internal::components::ObjectTransform;
using tei::internal::ecs::Object;

ObjectTransform const ObjectTransform::ROOT{};

void OnEnable(ObjectTransform& transform, Object const& object)
{
	if (auto & parent{ object.GetParent() })
		transform.parent = &parent.GetComponent<ObjectTransform>();
	OnUpdate(transform);
}

void OnUpdate(ObjectTransform& transform)
{
	//if (tranform.requiresUpdate)
	transform.world = transform.local + transform.parent->world;
}
