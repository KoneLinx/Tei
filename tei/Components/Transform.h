#pragma once

#include <tei/unit.h>
#include <tei/utility.h>

namespace tei::internal::ecs
{
	class Object;
}

namespace tei::internal::components
{

	struct TransformAccess;

	class ObjectTransform : public tei::internal::utility::Observed<unit::Transform>
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

void OnEnable(std::nullptr_t, tei::internal::components::ObjectTransform&, tei::internal::ecs::Object const&);
void OnUpdate(std::nullptr_t, tei::internal::components::ObjectTransform&);
