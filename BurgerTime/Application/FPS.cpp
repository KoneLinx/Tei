#include "pch.h"
#include "FPS.h"

void burger::FPS::OnUpdate()
{
	m_Average = (Time->frame.delta + m_Average * m_HistoryFactor) / (m_HistoryFactor + 1);

	auto& [textrenderer] = Refs();
	textrenderer.Text() = std::to_string(int(1 / m_Average.count()));
}
