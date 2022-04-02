#include "Transform.h"
#include "ComponentHelpers.h"

using namespace tei::internal;
using namespace tei::internal::components;
using namespace tei::internal::ecs;
using namespace tei::internal::utility;

struct tei::internal::components::TransformAccess
{
	static void Enable(ObjectTransform& transform, Object const& object)
	{
		transform.parent = FindParentComponent<ObjectTransform>(object);
		Update(transform);
	}

	static void Update(ObjectTransform& transform)
	{
		transform.world.checkout();
		if (transform.parent)
		{
			if (transform.parent->world.check() | transform.checkout()) // we want bitwise or
				transform.world = *transform.parent->world * *transform;
		}
		else
		{
			if (transform.checkout())
				transform.world = unit::Transform{} * *transform;
		}
	}
};


void OnEnable(std::nullptr_t, ObjectTransform& transform, Object const& object)
{
	TransformAccess::Enable(transform, object);
}

void OnUpdate(std::nullptr_t, ObjectTransform& transform)
{
	TransformAccess::Update(transform);
}
