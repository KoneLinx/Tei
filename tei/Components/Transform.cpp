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
			if (transform.parent->world.check() | transform.checkout())
				transform.world = *transform.parent->world * *transform;
		}
		else
		{
			if (transform.checkout())
				transform.world = unit::Transform{} * *transform;
		}
	}
};


void OnEnable(tei::internal::Internal, ObjectTransform& transform, Object const& object)
{
	TransformAccess::Enable(transform, object);
}

void OnUpdate(tei::internal::Internal, ObjectTransform& transform)
{
	TransformAccess::Update(transform);
}
