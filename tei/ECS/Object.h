#pragma once

#include <vector>
#include <memory>
#include <ranges>
#include <array>
#include <span>
#include <list>

#include <tei/internal/utility.h>

namespace tei::internal::ecs
{

	enum struct Message
	{
		INIT,
		CLEANUP,
		ENABLE,
		DISABLE,
		UPDATE,
		RENDER,
		_COUNT
	};

	class Object final
	{
	public:

		Object(Object* pParent, bool active);
		~Object();

		Object(const Object& other) = delete;
		Object& operator = (const Object& other) = delete;

		Object(Object&& other) = default;
		Object& operator = (Object&& other) = default;

		// Test if object is valid and active
		operator bool() const noexcept;

		// Set update state
		void SetState(bool active) noexcept;

		// Add a component
		template <typename Data>
		Data& AddComponent(Data data = {});
		// Get a component (throws if not present)
		template <typename Data>
		Data& GetComponent() const;
		// Remove a component (throws if not present)
		template <typename Data>
		Data RemoveComponent();

		// Add a child object
		Object& AddChild(bool active = true);
		// Remove a child object
		void RemoveChild(Object const& child);

		// Guaranteed, except for root (scene). Null reference for root. 
		Object& GetParent() const noexcept;

		// View of all children
		auto GetAllChildren() const noexcept;
		// View of all active children
		auto GetActiveChildren() const noexcept;
		// View of all inactive children
		auto GetInactiveChildren() const noexcept;

		// Send message down the hirarchy
		void Do(Message);

	private:

		template <typename Data = void>
		struct Component;

		using Handle = void(*)(Component<>&, Message, Object&);

		Object* m_pParent;
		std::vector<std::pair<std::unique_ptr<Component<>>, Handle>> m_Components;
		std::list<Object> m_Children;

		bool m_Active;
		bool m_State;
		bool m_Initialised;

		void AddComponent(Component<>*, Handle);
		typename decltype(m_Components)::const_iterator GetComponent(Handle) const;
		std::unique_ptr<Component<>> ExtractComponent(Handle);

	};

}

namespace tei::external::ecs
{
	using tei::internal::ecs::Object;
}

namespace tei::internal::ecs
{

	inline Object::operator bool() const noexcept
	{
		return this != nullptr && m_Active;
	}
	
	// Add a component
	template<typename Data>
	inline Data& Object::AddComponent(Data data)
	{
		Component<Data>* pComponent{ new Component<Data>{ std::move(data) } };
		this->AddComponent(pComponent, &Component<Data>::Handle);
		return pComponent->data;
	}
	
	// Get a component (throws if not present)
	template<typename Data>
	inline Data& Object::GetComponent() const
	{
		return static_cast<Component<Data>&>(*GetComponent(&Component<Data>::Handle)->first).data;
	}
	
	// Remove a component (throws if not present)
	template<typename Data>
	inline Data Object::RemoveComponent()
	{
		// unique_ptr is to be discarded
		return std::move(static_cast<Component<Data>&>(*ExtractComponent(&Component<Data>::Handle)).data);
	}
	
	// Guaranteed, except for root (scene). Null reference for root. 
	inline Object& Object::GetParent() const noexcept
	{
		return *m_pParent;
	}
	
	// View of all children
	inline auto Object::GetAllChildren() const noexcept
	{
		return std::views::all(m_Children);
	}
	
	// View of all active children
	inline auto Object::GetActiveChildren() const noexcept
	{
		return std::views::filter(GetAllChildren(), std::identity{});
	}
	
	// View of all inactive children
	inline auto Object::GetInactiveChildren() const noexcept
	{
		return std::views::filter(GetAllChildren(), std::logical_not{});
	}

	template <typename Data>
	struct Object::Component final : Object::Component<>
	{
		Data data;

		Component(Data data);

		static void Handle(Component<>&, Message, Object&);
	};

	template <>
	struct Object::Component<void>
	{
		virtual ~Component() = default;
	};

	template<typename Data>
	inline Object::Component<Data>::Component(Data data)
		: data{ std::move(data) }
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
		case Message::RENDER:
			if constexpr (requires { ::OnRender(data, parent); })
				::OnRender(data, parent);
			else 
			if constexpr (requires { ::OnRender(data); })
				::OnRender(data);
			break;
		}
		//[] <size_t ... MESSAGE> (Component& data, Message message, std::index_sequence<MESSAGE...>)
		//{
		//	((message == static_cast<Message>(MESSAGE) && (On<static_cast<Message>(MESSAGE)>(data.data), true)) || ...);
		//}
		//(static_cast<Component&>(data), message, std::make_index_sequence<static_cast<size_t>(Message::_COUNT)>{});
	}

}
