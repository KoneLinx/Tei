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

		Object(Object* pParent, bool active);

		friend Object CreateRoot();

	public:

		~Object();

		Object(Object&& other) = default;
		Object& operator = (Object&& other) = default;

		bool IsRoot() const noexcept;
		bool IsActive() const noexcept;

		operator bool() const noexcept;

		// Set update state
		void SetState(bool active) noexcept;

		// Add a component
		template <typename Data>
		Data& AddComponent(Data data = {});
		// Get a component (throws if not present)
		template <typename Data>
		Data* GetComponent() const noexcept;
		// Remove a component (throws if not present)
		template <typename Data>
		auto RemoveComponent();

		// Add a child object
		Object& AddChild(bool active = true);
		// Remove a child object
		void RemoveChild(Object const& child);

		// Guaranteed, except for root (scene). Null reference for root. 
		Object& GetParent() noexcept;
		Object const& GetParent() const noexcept;

		// View of all children
		auto GetAllChildren() noexcept;
		auto GetAllChildren() const noexcept;
		// View of all active children
		auto GetActiveChildren() noexcept;
		auto GetActiveChildren() const noexcept;
		// View of all inactive children
		auto GetInactiveChildren() noexcept;
		auto GetInactiveChildren() const noexcept;

		// Send message down the hirarchy
		void Do(Message);

	private:
		
		Object(const Object& other);
		Object& operator = (const Object& other) = delete;

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
		typename decltype(m_Components)::const_iterator GetComponent(Handle) const noexcept;
		std::unique_ptr<Component<>> ExtractComponent(Handle);

	};

	Object CreateRoot();

}

namespace tei::external::ecs
{
	using tei::internal::ecs::Object;
}

namespace tei::internal::ecs
{

	inline bool Object::IsRoot() const noexcept
	{
		return m_pParent == nullptr;
	}
	
	inline bool Object::IsActive() const noexcept
	{
		return m_Active;
	}
	
	inline Object::operator bool() const noexcept
	{
		return IsActive();
	}

	template<typename Data>
	inline Data& Object::AddComponent(Data data)
	{
		Component<Data>* pComponent{ new Component<Data>{ std::move(data) } };
		this->AddComponent(pComponent, &Component<Data>::Handle);
		return pComponent->data;
	}
	
	template<typename Data>
	inline Data* Object::GetComponent() const noexcept
	{
		auto it{ GetComponent(&Component<Data>::Handle) };
		if (it == m_Components.end())
			return nullptr;
		else
			return &static_cast<Component<Data>&>(*it->first).data;
	}
	
	template<typename Data>
	inline auto Object::RemoveComponent()
	{
		// unique_ptr is to be discarded
		auto uptr{ ExtractComponent(&Component<Data>::Handle) };
		if constexpr (std::movable<Data>)
			return std::move(static_cast<Component<Data>&>(*uptr).data);
		else
			return; // void, unable to move
	}
	
	inline Object& Object::GetParent() noexcept
	{
		return *m_pParent;
	}

	inline Object const& Object::GetParent() const noexcept
	{
		return *m_pParent;
	}
	
	inline auto Object::GetAllChildren() noexcept
	{
		return std::views::all(m_Children);
	}
	
	inline auto Object::GetActiveChildren() noexcept
	{
		return std::views::filter(GetAllChildren(), std::identity{});
	}
	
	inline auto Object::GetInactiveChildren() noexcept
	{
		return std::views::filter(GetAllChildren(), std::logical_not{});
	}

	inline auto Object::GetAllChildren() const noexcept
	{
		return std::views::all(m_Children);
	}
	
	inline auto Object::GetActiveChildren() const noexcept
	{
		return std::views::filter(GetAllChildren(), std::identity{});
	}
	
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

		virtual std::unique_ptr<Component<>> Clone() const override;
	};

	template <>
	struct Object::Component<void>
	{
		virtual ~Component() = default;

		virtual std::unique_ptr<Component<>> Clone() const = 0;
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
