#pragma once

#include <utility>
#include <vector>
#include <memory>
#include <ranges>
#include <array>
#include <span>
#include <list>
#include <typeindex>

#include <tei/internal/utility.h>

namespace tei::internal::scene
{
	class SceneManager;
}

namespace tei::internal::ecs
{

	enum struct Message
	{
		INIT,
		CLEANUP,
		ENABLE,
		DISABLE,
		UPDATE,
		FIXEDUPDATE,
		RENDER
	};

	namespace detail
	{

		template <typename>
		concept Component = true;

	};

	class Object final
	{

		Object(Object* pParent, bool active);

		friend class scene::SceneManager;

	public:

		~Object();

		Object(Object&& other) = default;
		Object& operator = (Object&& other) = default;

		bool IsRoot() const;
		bool IsActive() const;

		// IsActive
		operator bool() const;

		// Set update state
		void SetState(bool active);

		// Add a component
		template <detail::Component Data> requires std::movable<Data>
		Data& AddComponent(Data data);
		template <detail::Component Data, typename ... Arg> requires std::constructible_from<Data, Arg...>
		Data& AddComponent(Arg&& ...);

		// Get a component (throws if not present)
		template <detail::Component Data>
		Data* GetComponent() const;

		// Remove a component (throws if not present)
		template <detail::Component Data>
		auto RemoveComponent();

		// Add a child object
		Object& AddChild(bool active = true);
		// Remove a child object
		void RemoveChild(Object const& child);

		// Guaranteed, except for root (scene). Null reference for root. 
		Object& GetParent();
		Object const& GetParent() const;

		// View of all children
		std::ranges::view auto GetAllChildren();
		std::ranges::view auto GetAllChildren() const;

		// View of all active children
		std::ranges::view auto GetActiveChildren();
		std::ranges::view auto GetActiveChildren() const;

		// View of all inactive children
		std::ranges::view auto GetInactiveChildren() const;

		// Send message down the hirarchy
		void Do(Message);

	private:
		
		Object(const Object& other);
		Object& operator = (const Object& other) = delete;

		template <typename Data = void>
		struct Component;

		using Handle = void(*)(Component<>&, Message, Object&);

		Object* m_pParent;
		std::vector<std::tuple<std::type_index, std::unique_ptr<Component<>>, Handle>> m_Components;
		std::vector<std::unique_ptr<Object>> m_Children;

		bool m_Active;
		bool m_State;
		bool m_Initialised;

		void AddComponent(std::type_index, Component<>*, Handle);
		typename decltype(m_Components)::const_iterator GetComponent(std::type_index) const;
		std::unique_ptr<Component<>> ExtractComponent(std::type_index);

	};

}

namespace tei::external::ecs
{
	using tei::internal::ecs::Object;
}

namespace tei::internal::ecs
{

	inline bool Object::IsRoot() const
	{
		return m_pParent == nullptr;
	}
	
	inline bool Object::IsActive() const
	{
		return m_Active;
	}
	
	inline Object::operator bool() const
	{
		return IsActive();
	}

	template<detail::Component Data> requires std::movable<Data>
	inline Data& Object::AddComponent(Data data)
	{
		return AddComponent<Data, Data>(std::move(data));
	}

	template<detail::Component Data, typename ...Arg> requires std::constructible_from<Data, Arg...>
	inline Data& ecs::Object::AddComponent(Arg&& ... arg)
	{
		Component<Data>* pComponent{ new Component<Data>{ std::forward<Arg>(arg)... } };
		this->AddComponent(typeid(Data), pComponent, &Component<Data>::Handle);
		return pComponent->data;
	}
	
	template<detail::Component Data>
	inline Data* Object::GetComponent() const
	{
		auto it{ GetComponent(typeid(Data)) };
		if (it == m_Components.end())
			return nullptr;
		else
			return &static_cast<Component<Data>&>(*std::get<1>(*it)).data;
	}
	
	template<detail::Component Data>
	inline auto Object::RemoveComponent()
	{
		[[maybe_unused]] auto uptr{ ExtractComponent(typeid(Data)) };
		if constexpr (std::movable<Data>)
			return std::move(static_cast<Component<Data>&>(*uptr).data);
		else
			return; // void, unable to move
	}
	
	inline Object& Object::GetParent() 
	{
		return *m_pParent;
	}

	inline Object const& Object::GetParent() const 
	{
		return *m_pParent;
	}

	inline std::ranges::view auto Object::GetAllChildren()
	{
		return std::views::transform(m_Children, utility::wrapped_ptr_projector<true>{});
	}
	inline std::ranges::view auto Object::GetAllChildren() const
	{
		return std::views::transform(m_Children, utility::wrapped_ptr_projector<true, true>{});
	}

	inline std::ranges::view auto Object::GetActiveChildren()
	{
		return std::views::filter(GetAllChildren(), std::identity{});
	}
	inline std::ranges::view auto Object::GetActiveChildren() const
	{
		return std::views::filter(GetAllChildren(), std::identity{});
	}
	
	inline std::ranges::view auto Object::GetInactiveChildren() const
	{
		return std::views::filter(std::views::transform(m_Children, utility::wrapped_ptr_projector<true>{}), std::logical_not{});
	}

	template <typename Data>
	struct Object::Component final : Object::Component<>
	{
		Data data;

		template <typename ... Arg>
		Component(Arg&& ... arg);

		static void Handle(Component<>&, Message, Object&);

		virtual std::unique_ptr<Component<>> Clone() const override;
	};

	template <>
	struct Object::Component<void>
	{
		virtual ~Component() = default;

		virtual std::unique_ptr<Component<>> Clone() const = 0;
	};

	template<typename Data>
	template <typename ... Arg>
	inline Object::Component<Data>::Component(Arg&& ... arg)
		: data{ std::forward<Arg>(arg) ... }
	{}

	template <typename Data>
	inline void Object::Component<Data>::Handle(Component<>& component, Message message, Object& parent)
	{
		Data& data{ static_cast<Component&>(component).data };
		switch (message)
		{
		case Message::INIT:
			if constexpr (requires { ::OnInitialize(data, parent); })
				::OnInitialize(data, parent);
			else 
			if constexpr (requires { ::OnInitialize(data); })
				::OnInitialize(data);
			break;
		case Message::CLEANUP:
			if constexpr (requires { ::OnCleanup(data, parent); })
				::OnCleanup(data, parent);
			else 
			if constexpr (requires { ::OnCleanup(data); })
				::OnCleanup(data);
			break;
		case Message::ENABLE:
			if constexpr (requires { ::OnEnable(data, parent); })
				::OnEnable(data, parent);
			else 
			if constexpr (requires { ::OnEnable(data); })
				::OnEnable(data);
			break;
		case Message::DISABLE:
			if constexpr (requires { ::OnDisable(data, parent); })
				::OnDisable(data, parent);
			else 
			if constexpr (requires { ::OnDisable(data); })
				::OnDisable(data);
			break;
		case Message::UPDATE:
			if constexpr (requires { ::OnUpdate(data, parent); })
				::OnUpdate(data, parent);
			else 
			if constexpr (requires { ::OnUpdate(data); })
				::OnUpdate(data);
			break;
		case Message::FIXEDUPDATE:
			if constexpr (requires { ::OnFixedUpdate(data, parent); })
				::OnFixedUpdate(data, parent);
			else 
			if constexpr (requires { ::OnFixedUpdate(data); })
				::OnFixedUpdate(data);
			break;
		case Message::RENDER:
			if constexpr (requires { ::OnRender(data, parent); })
				::OnRender(data, parent);
			else 
			if constexpr (requires { ::OnRender(data); })
				::OnRender(data);
			break;
		}
	}

	template <typename Data>
	inline std::unique_ptr<Object::Component<>> Object::Component<Data>::Clone() const
	{
		if constexpr (std::copyable<Data>)
			return std::unique_ptr<Component<>>{ new Component<Data>{ data } };
		else
			throw utility::TeiRuntimeError{ "While cloning Object: Component not copyable", typeid(Data).name() };
	}

}
