#pragma once

#include <tei.h>

namespace burger
{

	class Hitbox : public tei::components::RefComponent<tei::components::ObjectTransform>, public tei::components::Subject
	{
	public:

		Hitbox(std::shared_ptr<std::vector<Hitbox*>> others);
		~Hitbox();

		void OnEnbale(tei::ecs::Object&);
		void OnDisable();

		void OnUpdate();

		bool CollidesWith(Hitbox const& other) const;

		auto OverlappingObjects() const;

		struct Hit
		{
			enum struct State : bool
			{
				LEAVE = false, 
				ENTER = true
			};
			using enum State;

			tei::ecs::Object& object1;
			tei::ecs::Object& object2;

			State state;
		};

	private:

		void DelistSelf();
		void EnlistSelf();

		tei::ecs::Object* m_pParent;
		std::shared_ptr<std::vector<Hitbox*>> m_Objects;
		std::vector<Hitbox*> m_Overlaps;
	};
	
	auto Hitbox::OverlappingObjects() const
	{
		return std::views::transform(
			m_Overlaps, 
			[] (Hitbox* p) -> tei::ecs::Object& 
			{
				return *p->m_pParent; 
			}
		);
	}

}