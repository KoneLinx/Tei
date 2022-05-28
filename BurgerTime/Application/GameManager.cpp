#include "pch.h"
#include "GameManager.h"
#include "Menu.h"

GameManager::GameManager()
{}

void GameManager::OnEnable(tei::ecs::Object& parent)
{
	m_pObject = &parent.AddChild();
	m_pObject->AddComponent(
		tei::Event->AddObserver(
			[this](GameReset)
			{
				m_Reset = true;
				m_pObject->GetParent().SetState(false);
			}
		)
	);
	CreateMainMenu(*m_pObject);
}

void GameManager::OnDisable(tei::ecs::Object& parent)
{
	if (m_Reset)
	{
		m_Reset = false;
		parent.RemoveChild(*m_pObject);
		parent.SetState(true);
	}
}

