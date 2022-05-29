#include "teipch.h"
#include "WindowAlignment.h"

#include <tei/application.h>
#include <tei/event.h>

tei::internal::components::WindowAlignment::WindowAlignment(unit::Position offset, unit::Position pivot)
	: m_Offset{offset}
	, m_Pivot{pivot}
{}

void tei::internal::components::WindowAlignment::OnEnable()
{
	METRICS_TIMEBLOCK;

	auto& [transform] = Refs();
	
	auto update = [&] (application::WindowPropertyChangedEvent)
	{
		transform.get().position = m_Pivot / 2.f * application::ApplicationService->GetWindow().transform.scale + m_Offset;
	};
	update({});

	m_Handle = events::Event->AddObserver(std::move(update));
}

void tei::internal::components::WindowAlignment::OnDisable()
{
	METRICS_TIMEBLOCK;

	m_Handle.Clear();
}
