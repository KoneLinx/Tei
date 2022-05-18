#include "pch.h"
#include "Ingredient.h"

#include "Level.h"
#include "Hitbox.h"
#include "Player.h"
#include "StaticEntity.h"

#include <numbers>

using namespace tei;
using namespace tei::resource;
using namespace tei::components;

constexpr inline float visualOffset = -.2f;

tei::ecs::Object& IngredientEnity::Create(tei::ecs::Object& parent, IngredientData const& data)
{
	auto& object = parent.AddChild();

	auto& self = object.AddComponent<IngredientEnity>();

	self.m_pData = &data;

	{
		auto& visualObject = object.AddChild();
		self.m_pVisualTransform = &visualObject.AddComponent<ObjectTransform>({ { unit::Position{ 0, visualOffset }, unit::Scale{ 2.75f }} });
		visualObject.AddComponent<TextureRenderComponent>();
		visualObject.AddComponent(data.texture);
	}

	for(int i{}; i < 3; ++i)
	{
		auto& triggerObject = object.AddChild();
		triggerObject.AddComponent<ObjectTransform>(unit::Position{ i - 1, 0 });
		triggerObject.AddComponent(Box{ .5f, .5f });
		auto& hitbox = triggerObject.AddComponent<Hitbox>();
		triggerObject.AddComponent(
			hitbox.AddObserver(
				[i, &self] (Hitbox::Hit const& hit)
				{
					if (hit.state == hit.ENTER)
					if (auto pAnima{ hit.object.HasComponent<Anima>() })
					if (/*is player*/true)
					{
						self.Pressed(i);
					}
				}
			)
		);
	}

	{
		auto& box = object.AddComponent(Box{ 3, .6f });
		auto& hitbox = object.AddComponent<Hitbox>();
		object.AddComponent(
			hitbox.AddObserver(
				[&self, &box] (Hitbox::Hit const& hit)
				{
					if (hit.state != hit.ENTER || self.m_IsOnPlate || !self.m_Falling)
						return;

					if (auto pEntity{ hit.object.HasComponent<StaticEntity>() })
					{
						auto& [transform] = self.Refs();

						if (pEntity->Type() == StaticEntityData::PLATE)
						{
							self.m_Timer = 0_s;
							self.m_IsOnPlate = true;
							transform.get().position = hit.object.GetComponent<ObjectTransform>()->position;
							transform.get().position.y += 0.4f;
						}
						if (pEntity->Type() == StaticEntityData::SHELF)
						{
							self.m_Timer = 0_s;
							transform.get().position = hit.object.GetComponent<ObjectTransform>()->position;
						}

					}

					if (auto pIngredient{ hit.object.HasComponent<IngredientEnity>() })
					{
						auto& [transform] = self.Refs();
						
						self.m_Timer = 0_s;
						if (pIngredient->m_IsOnPlate)
						{
							self.m_IsOnPlate = true;
							transform.get().position = hit.object.GetComponent<ObjectTransform>()->position;
							transform.get().position.y += 0.65f;
						}
						else
						{
							auto& [otherTransform] = pIngredient->Refs();
							transform.get().position = otherTransform->position;
							otherTransform.get().position.y -= box.y * 1.1f;
							pIngredient->Pressed(-1);
							(*pIngredient->m_pVisualTransform).get().position.y = (*self.m_pVisualTransform).get().position.y += box.y * 1.f;
						}

					}
				}
			)
		);
	}

	return object;
}

void IngredientEnity::OnUpdate()
{
	if (m_Timer)
		m_Falling ^= true;

	if (m_Falling)
	{
		auto& [transform] = Refs();
		transform.get().position.y -= 2.f * Time->frame.delta.count();
	}

	if (auto& t{ *m_pVisualTransform }; t->position.y > visualOffset)
	{
		t.get().position.y = std::max(visualOffset, t->position.y - 2.f * Time->frame.delta.count());
	}
}

void IngredientEnity::Pressed(int index)
{
	if (m_Falling || m_IsOnPlate)
		return;

	if (index >= 0)
	{
		m_Pressed.set(index);
	}

	if (m_Pressed.all() || index < 0)
	{
		m_Timer = 0_s;
		m_Pressed = {};
	}

	float r{};
	float y{ visualOffset };
	float const rinc{ std::numbers::pi_v<float> / 24.f };
	float const yinc{ -.25f };

	if (m_Pressed.test(0))
		r -= rinc, y += yinc;
	if (m_Pressed.test(2))
		r += rinc, y += yinc;
	if (m_Pressed.test(1))
		r += r, y += yinc;

	m_pVisualTransform->get().rotation.r = r;
	m_pVisualTransform->get().position.y = y;
}

void IngredientEnity::Update()
{

	//if (m_Falling)
	//{
	//	float const yspeed{ 2.f };

	//	auto& [transform] = Refs();
	//	transform.get().position.y -= yspeed * Time->thread->delta.count();
	//}

}
