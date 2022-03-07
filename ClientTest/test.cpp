
#include "TestComponent.h"

#include <tei.h>

using namespace tei::literals;

struct Timer {};

void OnUpdate(Timer)
{
#ifdef _DEBUG
	if (tei::Time->frame.now > 3_s)
		tei::Application->Quit();
#endif
}

void TeiClientInit()
{

	puts("Client init");

	tei::Scene->AddComponent<Timer>();

	tei::Scene->AddComponent<std::string>();

	{
		auto& c1 = tei::Scene->AddChild();
		c1.AddComponent<std::string>();

		c1.AddChild().AddComponent<std::string>();
		c1.AddChild().AddComponent<std::string>();
	}

	{
		auto& c2 = tei::Scene->AddChild();
		c2.AddComponent<std::string>();

		{
			auto& c21 = c2.AddChild();
			c21.AddComponent<std::string>();

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