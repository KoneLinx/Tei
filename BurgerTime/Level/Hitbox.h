#pragma once

#include <tei.h>

	using Box = tei::unit::Dimentions;


	class Hitbox : public tei::components::RefComponent<tei::components::ObjectTransform, Box>, public tei::components::Subject
	{
	public:

		using List = std::vector<Hitbox*>;

		//Hitbox();
		Hitbox(/*std::shared_ptr<std::vector<Hitbox*>> others*/);
		~Hitbox();

		Hitbox(Hitbox const&) = delete;
		Hitbox(Hitbox &&) = default;
		
		Hitbox& operator = (Hitbox const&) = delete;
		Hitbox& operator = (Hitbox &&) = default;

		void OnEnable(tei::ecs::Object&);
		void OnDisable();

		void OnUpdate();

		void ReEnter();

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

#ifdef _DEBUG
		void OnInitialize(tei::ecs::Object& object);
#endif

	private:

		void DelistSelf();
		void EnlistSelf();

		void Delist(Hitbox const& other);

		tei::ecs::Object* m_pParent;
		List* m_pObjects;
		std::vector<Hitbox*> m_Overlaps;
	};
