#include "pch.h"
#include "Hitbox.h"

Hitbox::Hitbox(/*std::shared_ptr<std::vector<Hitbox*>> others*/)
	: m_pParent{ nullptr }
	, m_pObjects{ /*std::move(others)*/ }
	, m_Overlaps{ }
{}

Hitbox::~Hitbox()
{
	if (m_pObjects)
		DelistSelf();
}

void Hitbox::OnEnable(tei::ecs::Object& parent)
{
	m_pParent = &parent;

	List* pVec{};
	Object const* pParent{ &parent };

	while ((pVec = pParent->HasComponent<std::vector<Hitbox*>>()) == nullptr)
		pParent = &pParent->GetParent();

	m_pObjects = pVec;

	EnlistSelf();
}


void Hitbox::OnDisable()
{
	if (m_pObjects)
		DelistSelf();
	m_pObjects = {};
}

void Hitbox::OnUpdate()
{
	if (!m_pObjects)
		return;

	if (std::ranges::subrange matches{ std::ranges::next(std::ranges::find(*m_pObjects, this)), std::ranges::end(*m_pObjects) }; !matches.empty())
	{
		for (auto& pMatch : m_Overlaps)
		{
			if (!CollidesWith(*pMatch))
			{
				pMatch->Notify(Hit{ *m_pParent, Hit::LEAVE });
				this->Notify(Hit{ *pMatch->m_pParent, Hit::LEAVE });
				pMatch = {};
			}
		}
		auto [from, to] = std::ranges::remove(m_Overlaps, nullptr);
		m_Overlaps.erase(from, to);
		auto overlaps{ tei::utility::RangePerIndex(m_Overlaps) };

		for (auto pMatch : matches)
		{
			if (CollidesWith(*pMatch) && std::ranges::find(overlaps, pMatch) == overlaps.end())
			{
				m_Overlaps.push_back(pMatch);
				pMatch->Notify(Hit{ *m_pParent, Hit::ENTER });
				this->Notify(Hit{ *pMatch->m_pParent, Hit::ENTER });
			}
		}
	}
}

void Hitbox::ReEnter()
{
	for (auto pMatch : m_Overlaps)
	{
		pMatch->Notify(Hit{ *m_pParent, Hit::ENTER });
		this->Notify(Hit{ *pMatch->m_pParent, Hit::ENTER });
	}
}

bool IsOverlapping(tei::unit::Transform const& a, tei::unit::Transform const& b)
{
#if false /*circle*/
	auto min = (a.scale.x + a.scale.y + b.scale.x + b.scale.y) / 2;
	auto diff = b.position - a.position;
	return min * min > glm::dot(diff, diff);
#else /*rectangle*/
	auto min = a.scale + b.scale / 2.f;
	auto diff{ glm::abs(a.position - b.position) };
	return min.x > diff.x && min.t > diff.y;
#endif
}

bool IsOverlapping(tei::unit::Transform const& amat, tei::unit::Dimentions adim, tei::unit::Transform const& bmat, tei::unit::Dimentions bdim)
{
	tei::unit::Rectangle 
		arect{
			amat.position - adim * amat.scale / 2.f,
			amat.position + adim * amat.scale / 2.f
		},
		brect{
			bmat.position - bdim * bmat.scale / 2.f,
			bmat.position + bdim * bmat.scale / 2.f
		};

	return arect[1].x > brect[0].x && arect[0].x < brect[1].x
		&& arect[1].y > brect[0].y && arect[0].y < brect[1].y;
}

bool Hitbox::CollidesWith(Hitbox const& other) const
{
	auto& [selfTransform, selfBox] = Refs();
	auto& [otherTransform, otherBox] = other.Refs();
	return IsOverlapping(selfTransform.world, selfBox, otherTransform.world, otherBox);
}

#ifdef _DEBUG
void Hitbox::OnInitialize([[maybe_unused]] tei::ecs::Object& object)
{
	//auto& box = object.GetComponent<Box>();
	//auto texture = tei::Resources->LoadShared<tei::resource::Texture>("resources/Box_Debug.png");
	//object.AddChild().AddComponents(
	//	tei::components::ObjectTransform{ tei::unit::Scale{ box.x, box.y } },
	//	std::move(texture),
	//	tei::components::TextureRenderComponent{}
	//);
}
#endif

void Hitbox::DelistSelf()
{
	if (auto it{ std::ranges::find(*m_pObjects, this) }; it != std::ranges::end(*m_pObjects))
	{
		std::ranges::for_each(
			m_pObjects->begin(), it,
			[this](Hitbox* pHitbox)
			{
				pHitbox->Delist(*this);
			}
		);
		m_pObjects->erase(it);
	}
}

void Hitbox::EnlistSelf()
{
	if (std::ranges::find(*m_pObjects, this) == std::ranges::end(*m_pObjects))
		m_pObjects->push_back(this);
}

void Hitbox::Delist(Hitbox const& other)
{
	if (auto it = std::ranges::find(m_Overlaps, &other); it != std::ranges::end(m_Overlaps))
		m_Overlaps.erase(it);
}
