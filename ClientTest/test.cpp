
#include "TestComponent.h"

#include <tei.h>

#include <iostream>

using namespace tei::literals;

struct Timer {};

void OnUpdate(Timer)
{
	if (tei::Time->frame.now > 3_s)
		tei::Application->Quit();
}

void OnRender(tei::components::ObjectTransform const& t)
{
	std::cout << '(' << t.world.position.x << ", " << t.world.position.y << ')' << std::endl;
}

void TeiClientInit()
{
	using tei::components::ObjectTransform;

	puts("Client init");

	tei::Scene->AddComponent<Timer>();
	tei::Scene->AddComponent(ObjectTransform{});

	tei::Scene->AddComponent<std::string>();

	{
		auto& c1 = tei::Scene->AddChild();
		c1.AddComponent<std::string>();
		c1.AddComponent(ObjectTransform{ tei::unit::Position{1, 0} });
			
		c1.AddChild().AddComponent<std::string>();
		c1.AddChild().AddComponent<std::string>();
	}

	{
		auto& c2 = tei::Scene->AddChild();
		c2.AddComponent<std::string>();
		c2.AddComponent(ObjectTransform{ tei::unit::Position{0, 1} });

		{
			auto& c21 = c2.AddChild();
			c21.AddComponent<std::string>();
			c21.AddComponent(ObjectTransform{ tei::unit::Position{1, 0} });

			c21.AddChild().AddComponent<std::string>();
			c21.AddChild().AddComponent<std::string>();
			c21.AddChild().AddComponent<std::string>();
		}

		c2.AddChild().AddComponent<std::string>();
	}

}

void TeiClientCleanup()
{

	puts("Client cleanup");

}