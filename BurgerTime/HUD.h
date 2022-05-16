#pragma once

#include <tei.h>

namespace burger
{

	class IconbarDisplay
	{

	public:

		void OnInitialize(tei::ecs::Object& object);
		void OnEnable();
		void OnDisable();

	private:

		void LoadHealth(tei::ecs::Object& object);
		void LoadLevel(tei::ecs::Object& object);

		std::function<void(int value)> m_Update;


	};

}
