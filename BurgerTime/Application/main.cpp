#include "pch.h"

#include "FPS.h"

#include "../Loader/LevelLoader.h"

void TeiClientInit()
{

	auto& scene = tei::Scenes->AddScene(true);

	{
		auto& fps = scene.AddChild();
		fps.AddComponent<burger::ObjectTransform>(tei::unit::Position{ 10, -10 });
		fps.AddComponent(tei::Resources->LoadShared<tei::resource::Font>("resources/Lingua.otf", 11));
		fps.AddComponent(burger::WindowAlignment{ { -14, -10 }, { 1, 1 } });
		fps.AddComponent<burger::FPS>();
	}

	tei::Application->SetWindowProperty(tei::application::WindowProperty::RESIZABLE);

	test();

}

void TeiClientCleanup()
{

}