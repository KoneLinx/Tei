#pragma once

#include "RefComponent.h"
#include "Transform.h"
#include "Subject.h"

#include <tei/unit.h>

namespace tei::internal::components
{

	class WindowAlignment : public RefComponent<ObjectTransform>
	{
	public:

		WindowAlignment(unit::Position offset, unit::Position pivot = {});

		void OnEnable();
		void OnDisable();

	private:

		unit::Position m_Offset;
		unit::Position m_Pivot;
		Subject::ObserverHandle m_Handle{};

	};

}

namespace tei::external::components
{
	using tei::internal::components::WindowAlignment;
}