#pragma once

#include <tei.h>

namespace burger
{

	using Box = tei::unit::Dimentions;

	class Hitbox : public tei::components::RefComponent<tei::components::ObjectTransform, Box>, public tei::components::Subject
	{
	public:

		Hitbox();
		Hitbox(std::shared_ptr<std::vector<Hitbox*>> others);
		~Hitbox();

		Hitbox(Hitbox&&) = default;

		void OnEnbale(tei::ecs::Object&);
		void OnDisable();

		void OnUpdate();

		bool CollidesWith(Hitbox const& other) const;

		//auto OverlappingObjects() const;

		struct Hit
		{
			enum struct State : bool
			{
				LEAVE = false, 
				ENTER = true
			};
			using enum State;

			tei::ecs::Object const& object;

			State state;
		};

	private:

		void DelistSelf();
		void EnlistSelf();

		tei::ecs::Object* m_pParent;
		std::shared_ptr<std::vector<Hitbox*>> m_Objects;
		std::vector<Hitbox*> m_Overlaps;
	};
	
	//auto Hitbox::OverlappingObjects() const
	//{
	//	return std::views::transform(
	//		m_Overlaps, 
	//		[] (Hitbox* p) -> tei::ecs::Object& 
	//		{
	//			return *p->m_pParent; 
	//		}
	//	);
	//}

}