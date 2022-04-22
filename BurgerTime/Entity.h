#pragma once

#include <functional>

#include <tei.h>


class Entity : public tei::components::RefComponent<tei::components::ObjectTransform>
{
public:

	void SetStateHandler(std::function<void(Entity&)> handler = {});

	void Update();

private:

	std::function<void(Entity&)> m_StateHandler{};

};

class EntityPlayfieldHandler
{

	void HandleUpdate(Entity& entity);

public:

	static EntityPlayfieldHandler& AddTo(Entity& controller);

	void SetInputController(std::function<tei::unit::Vec2()> controller)
	{ m_Controller = std::move(controller); }

private:

	std::function<tei::unit::Vec2()> m_Controller{};

};

//class EntityEnemyAIController
//{
//
//	EntityEnemyAIController(int type);
//
//	tei::unit::Vec2 QueryDecision();
//
//};

class EntityPlayerController
{

	EntityPlayerController(int id);

public:

	static void AddTo(EntityController& controller, int id);

	tei::unit::Vec2 QueryInput();

private:

	tei::components::InputComponent m_Input;

	enum struct Action
	{
		NONE = 0, LEFT, RIGHT, UP, DOWN, ATTACK
	}
	m_Action{};

	int id;

};

#include "Json.h"

namespace burger::entity
{

	using namespace tei;
	using namespace common;
	using namespace components;


	class EntityController
	{
	
	public:

		void OnUpdate();

		struct Input
		{
			unit::Vec2 direction;
			Observed<void> attack;
		};

		void SetInput(Input);
	
	};

	class PlayerInput : RefComponent<EntityController>
	{
		void OnUpdate();
	};

	class AIInput : RefComponent<EntityController>
	{
		void OnUpdate();
	};

	class Hitbox : RefComponent<ObjectTransform>
	{
	public:

		Hitbox(std::shared_ptr<std::vector<Hitbox*>> others);

		void OnEnbale(Object&);
		void OnDisable();

		void OnUpdate();

		bool CollidesWith(Hitbox const& other) const;

		std::optional<Object&> HitObject() const;

		Object* m_pParent;
		std::shared_ptr<std::vector<Hitbox*>> m_Objects;
		Observed<Hitbox*> m_Hit;
	};

	class Entity : RefComponent<EntityController, SpriteRenderComponent>
	{

		static void LoadFromObject(nlohmann::json const& data, Object& parent);

	};

}