#pragma once

struct GameReset{};

class GameManager
{

public:

	GameManager();

	void OnEnable(tei::ecs::Object& parent);
	void OnDisable(tei::ecs::Object& parent);

private:

	tei::ecs::Object* m_pObject;
	bool m_Reset;

};

