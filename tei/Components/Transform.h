#pragma once

#include <tei/unit.h>
#include <tei/internal/ecs.h>

namespace tei::internal::components
{

	//struct TransformData : unit::Transform
	//{
	//	using unit::Transform::Transform;
	//	using unit::Transform::operator=;

	//	unit::Transform world{};
	//};

	struct TransformAccess;

	class ObjectTransform : public utility::Observed<unit::Transform>
	{
	public:

		using utility::Observed<unit::Transform>::Observed;
		using utility::Observed<unit::Transform>::operator=;

		utility::Observed<unit::Transform> world;

	private:

		ObjectTransform* parent{};

		friend struct TransformAccess;
	};

}

namespace tei::external::components
{
	using tei::internal::components::ObjectTransform;
}

void OnEnable(tei::internal::Internal, tei::internal::components::ObjectTransform&, tei::internal::ecs::Object const&);
void OnUpdate(tei::internal::Internal, tei::internal::components::ObjectTransform&);
