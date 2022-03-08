#pragma once

#include <tei/unit.h>
#include <tei/internal/ecs.h>

namespace tei::internal::components
{

	struct ObjectTransform
	{
		ObjectTransform(unit::Transform local = {}) noexcept
			: local{ local }
		{}

		unit::Transform local;
		unit::Transform world{};

		static ObjectTransform const ROOT;
		ObjectTransform const* parent{ &ROOT };
		//bool requiresUpdate{ true };
	};

}

namespace tei::external::components
{
	using tei::internal::components::ObjectTransform;
}

void OnEnable(tei::internal::components::ObjectTransform&, tei::internal::ecs::Object const&);

void OnUpdate(tei::internal::components::ObjectTransform&);
