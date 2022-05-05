#pragma once

#include <tei.h>

namespace burger
{

	using namespace tei;
	using namespace components;

	class FPS : public RefComponent<TextRenderComponent>
	{
	public:

		FPS() = default;

		void OnUpdate();

	private:

		float m_HistoryFactor{ 2 };
		Clock::duration m_Average{};

	};

}
