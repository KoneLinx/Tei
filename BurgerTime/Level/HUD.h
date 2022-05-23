#pragma once

#include <tei.h>

#include "Level.h"

	class IconbarDisplay
	{

	public:

		void Update(int value);

		static tei::ecs::Object& CreateHealthDisplay(tei::ecs::Object& object, LevelData const& leveldata);
		static tei::ecs::Object& CreateLevelDisplay(tei::ecs::Object& object, LevelData const& leveldata);

	private:


		std::function<void(int value)> m_Update;

	};

	class BoundText
	{

		BoundText(tei::unit::Colour colour = { 1, 1, 1, 1 });

	};

