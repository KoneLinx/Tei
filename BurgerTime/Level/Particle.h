#pragma once

#include <tei.h>

struct ParticleData
{
	using Type = std::string_view;

	static inline constexpr Type HEARTH = "heart";
	static inline constexpr Type LEVEL_1 = "level_1";
	static inline constexpr Type LEVEL_5 = "level_5";
	static inline constexpr Type LEVEL_10 = "level_10";
	static inline constexpr Type CLOUD = "cloud";
	static inline constexpr Type SCORE_0 = "score_0";
	static inline constexpr Type SCORE_50 = "score_50";
	static inline constexpr Type SCORE_100 = "score_100";
	static inline constexpr Type SCORE_200 = "score_200";
	static inline constexpr Type SCORE_300 = "score_300";
	static inline constexpr Type SCORE_500 = "score_500";
	static inline constexpr Type SCORE_600 = "score_600";
	static inline constexpr Type SCORE_900 = "score_900";
	static inline constexpr Type SCORE_1000 = "score_1000";
	static inline constexpr Type SCORE_2000 = "score_2000";
	static inline constexpr Type SCORE_4000 = "score_4000";
	static inline constexpr Type SCORE_8000 = "score_8000";
	static inline constexpr Type SCORE_16000 = "score_16000";
	static inline constexpr Type SCORE_PLUS = "score_plus";

	Type type;
	
	std::string name{};
	
	tei::Resource<tei::resource::Texture> texture;
};

class Particle : tei::components::RefComponent<tei::components::ObjectTransform>
{
public:

	static tei::ecs::Object& Create(tei::ecs::Object& parent, tei::unit::Transform start, tei::unit::Transform increment, std::variant<tei::Resource<tei::resource::Texture>, tei::Resource<tei::resource::Sprite>> texture, tei::Clock::duration duration);

	void OnUpdate(tei::ecs::Object& object);
	void OnDisable(tei::ecs::Object& object);

private:

	Particle(tei::components::ObjectTransform*, tei::unit::Transform, tei::time::TimeOnce);

	tei::components::ObjectTransform* m_pVisualTransform{};
	tei::unit::Transform m_Increment{};
	tei::time::TimeOnce m_Timer{};

};

