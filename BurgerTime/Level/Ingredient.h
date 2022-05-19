#pragma once

#include <tei.h>

#include <bitset>

struct IngredientData
{
	using Type = std::string_view;

		static inline constexpr Type BUN_TOP = "bun_top";
		static inline constexpr Type BUN_BOTTOM = "bun_bottom";
		static inline constexpr Type PATTY = "patty";
		static inline constexpr Type LETTUCE = "lettuce";
		static inline constexpr Type CHEESE = "cheese";
		static inline constexpr Type TOMATO = "tomato";

	Type type;

	std::string name;

	Resource<resource::Texture> texture;

	bool unique{};
	int position{};
};

class IngredientEnity : public tei::components::RefComponent<tei::components::ObjectTransform> 
{
public:

	static tei::ecs::Object& Create(tei::ecs::Object& parent, IngredientData const& data);

	void OnUpdate();

	bool IsFalling() const { return m_Falling; };

private:

	IngredientData const* m_pData{};

	tei::components::ObjectTransform* m_pVisualTransform{};

	tei::time::TimeOnce m_Timer{};

	std::bitset<3> m_Pressed{};
	bool m_IsOnPlate : 1 { false };
	bool m_Falling : 1 {false};

	void Pressed(int index);

};

