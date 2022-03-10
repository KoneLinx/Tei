
#include "TestComponent.h"

#include <tei.h>

#include <iostream>

using namespace tei::literals;

struct Timer {};

void OnUpdate(Timer)
{
	if (tei::Time->frame.now > 6_s)
		tei::Application->Quit();
}

//void OnRender(tei::components::ObjectTransform const& t)
//{
//	std::cout << '(' << t.world.position.x << ", " << t.world.position.y << ')' << std::endl;
//}

void TeiClientInit()
{
	using namespace tei::components;

	tei::Application->SetWindowSize(640, 480);

	tei::Scene->AddComponent<Timer>();
	tei::Scene->AddComponent<ObjectTransform>();

	{
		auto& bgObject = tei::Scene->AddChild();
		bgObject.AddComponent(tei::Resources->LoadShared<tei::resource::Texture>("_smile.jpg"));
		bgObject.AddComponent<ObjectTransform>();
		bgObject.AddComponent<TextureRenderComponent>();
	}

}

void TeiClientCleanup()
{

}