#pragma once

#include <tei.h>

class Menu : public tei::components::Subject
{

public:

	using Event = int;

	Menu(std::vector<std::string_view> textFields);

	void OnInitialize(tei::ecs::Object&);

	void OnEnable();
	void OnDisable();

private:

	std::function<void(int)> m_Updater;
	std::any m_Storage;

};

tei::ecs::Object& CreateMainMenu(tei::ecs::Object& parent);

tei::ecs::Object& CreateScoreMenu(tei::ecs::Object& parent, tei::ecs::Object& level, Clock::duration durtation, bool failed);