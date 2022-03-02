
#include <tei.h>

#include "TestComponent.h"

void TeiClientInit()
{

	puts("Client init");

	using namespace std::literals;

	tei::Scene->AddComponent("Scene"sv);

	auto& c1 = tei::Scene->AddChild();

	c1.AddComponent("child 1"sv);

	tei::Scene->AddChild().AddComponent("child 1.1"sv);
	tei::Scene->AddChild().AddComponent("child 1.2"sv);

	auto& c2 = tei::Scene->AddChild();

	c1.AddComponent("child 2"sv);

	auto& c21 = c2.AddChild();
	c21.AddComponent("child 2.1"sv);

	c21.AddChild().AddComponent("child 2.1.1"sv);
	c21.AddChild().AddComponent("child 2.1.2"sv);
	c21.AddChild().AddComponent("child 2.1.3"sv);

	tei::Scene->AddChild().AddComponent("child 2.2"sv);


}

void TeiClientCleanup()
{

	puts("Client cleanup");

}