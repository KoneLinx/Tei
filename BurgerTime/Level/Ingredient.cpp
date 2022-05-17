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

tei::ecs::Object& IngredientEnity::Create(tei::ecs::Object& parent, IngredientData const& data)
{
	auto& object = parent.AddChild();

	auto& self = object.AddComponent<IngredientEnity>();

	self.m_pData = &data;

	{
		auto& visualObject = object.AddChild();
		visualObject.AddComponent<ObjectTransform>();
		visualObject.AddComponent<TextureRenderComponent>();
		visualObject.AddComponent(data.texture);
	}

	for(int i{}; i < 3; ++i)
	{
		auto& triggerObject = object.AddChild();
		triggerObject.AddComponent<ObjectTransform>(unit::Position{ i - 1, 0 });
		triggerObject.AddComponent(Box{ 1, 1 });
		auto& hitbox = triggerObject.AddComponent<Hitbox>();
		triggerObject.AddComponent(
			hitbox.AddObserver(
				[i, &self] (Hitbox::Hit const& hit)
				{
					if (hit.state == hit.ENTER && hit.object.HasComponent<PlayerEntity>())
					{
						self.Pressed(i);
					}
				}
			)
		);
	}

	{
		object.AddComponent(Box{});
		auto& hitbox = object.AddComponent<Hitbox>();
		object.AddComponent(
			hitbox.AddObserver(
				[&self] (Hitbox::Hit const& hit)
				{
					if (hit.state != hit.ENTER || self.m_IsOnPlate || !self.m_Falling)
						return;

					if (auto pEntity{ hit.object.HasComponent<StaticEntity>() })
					{
						if (pEntity->Type() == StaticEntityData::PLATE)
							self.m_Falling = false, self.m_IsOnPlate = true;
						if (pEntity->Type() == StaticEntityData::SHELF)
							self.m_Falling = false;
					}

					if (auto pIngredient{ hit.object.HasComponent<IngredientEnity>() })
					{
						self.m_Falling = false;
						if (pIngredient->m_IsOnPlate)
						{
							self.m_IsOnPlate = true;
						}
						else
							pIngredient->Pressed(-1);
					}
				}
			)
		);
	}

	return object;
}

void IngredientEnity::Pressed(int index)
{
	if (m_Falling)
		return;

	if (index >= 0)
	{
		m_Pressed.set(index);
	}

	if (m_Pressed.all() || index < 0)
	{
		m_Falling = true;
		m_Pressed = {};
	}

	float r{};
	float y{};
	float const rinc{ std::numbers::pi_v<float> / 12.f };
	float const yinc{ -0.2f };

	if (m_Pressed.test(0))
		r -= rinc, y -= yinc;
	if (m_Pressed.test(2))
		r += rinc, y -= yinc;
	if (m_Pressed.test(1))
		r += r, y -= yinc;

	auto& [transform] = Refs();
	transform.get().rotation.r = r;
	transform.get().position.y = y;
}

void IngredientEnity::Update()
{

	if (m_Falling)
	{
		float const yspeed{ 2.f };

		auto& [transform] = Refs();
		transform.get().position.y -= yspeed * Time->thread->delta.count();
	}

}
