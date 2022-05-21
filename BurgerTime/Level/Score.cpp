#include "pch.h"
#include "Score.h"
#include "Particle.h"
#include "Level.h"

#include <array>

using namespace tei;
using namespace resource;
using namespace unit;
using namespace components;
using namespace std::literals;

void Score::OnEnable(tei::ecs::Object const& object)
{
	ScoreManager* pManager{};
	tei::ecs::Object const* pParent{ &object };

	while ((pManager = pParent->HasComponent<ScoreManager>()) == nullptr)
		pParent = &pParent->GetParent();

	m_pManager = pManager;
}

void Score::OnDisable()
{
	m_pManager = nullptr;
}

ScoreManager::ScoreManager(LevelData const& leveldata)
	: m_pData{ &leveldata }
{}

void ScoreManager::OnEnable()
{
	auto makeHandle = [&]
	{
		return AddObserver(
			[&](Score::Event const& event)
			{
				m_Events.emplace_back(event);
			}
		);
	};
	m_Handle = std::make_shared<decltype(makeHandle())>(makeHandle());
}

void ScoreManager::OnUpdate(tei::ecs::Object& object)
{
	std::initializer_list scores = { std::pair
		{ 0    , ParticleData::SCORE_0     },
		{ 0    , ParticleData::SCORE_50    },
		{ 100  , ParticleData::SCORE_100   },
		{ 200  , ParticleData::SCORE_200   },
		{ 300  , ParticleData::SCORE_300   },
		{ 500  , ParticleData::SCORE_500   },
		{ 600  , ParticleData::SCORE_600   },
		{ 900  , ParticleData::SCORE_900   },
		{ 1000 , ParticleData::SCORE_1000  },
		{ 2000 , ParticleData::SCORE_2000  },
		{ 4000 , ParticleData::SCORE_4000  },
		{ 8000 , ParticleData::SCORE_8000  },
		{ 16000, ParticleData::SCORE_16000 },
		{ 20000, ParticleData::SCORE_PLUS  }
	};

	auto scoretex = [&] (long score) -> auto&
	{
		return m_pData->particles.at(
			std::ranges::prev(
				std::ranges::find_if(
					scores, 
					[=] (long key) 
					{ 
						return key > score;
					},
					[] (auto& pair) 
					{ 
						return pair.first;
					}
				)
			)->second
		).texture;
	};

	auto& [transform] = Refs();
	for (Score::Event const& event : m_Events)
	{
		if (auto const* pTransform = event.object.HasComponent<ObjectTransform>())
		{
			Position localPos = (pTransform->world->position - transform.world->position) / transform.world->scale;
			Particle::Create(object, { localPos, Scale{ 0.75f } }, { Position{ 0, .5 }, Scale{ 1.0025f } }, scoretex(event.score), 1_s);
		}
		m_Score += event.score;
	}
	if (!m_Events.empty())
	{
		m_Events.clear();
		Notify(m_Score);
	}
}

void ScoreManager::OnDisable()
{
	m_Handle = {};
}

ecs::Object& ScoreDisplay::Create(ecs::Object& parent)
{
	auto& object = parent.AddChild();

	object.AddComponents(
		Score{},
		ScoreDisplay{},
		TextRenderComponent{},
		Resources->LoadShared<Font>("resources/FalconPatrol.ttf", 26)
	);

	return object;
}

void ScoreDisplay::OnEnable()
{
	auto& [score, text] = Refs();

	text = "0"s;

	score->AddObserver(
		[&] (ScoreManager::ScoreType score)
		{
			text = std::to_string(score);
		}
	).Detach();
}
