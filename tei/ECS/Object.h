#pragma once

#include <vector>
#include <memory>
#include <typeindex>
#include <ranges>
#include <functional>

namespace tei::internal::scene
{
	class SceneManager;
}

namespace tei::internal::ecs
{

	namespace detail
	{
		struct MessageBase
		{};

		template <typename>
		concept ComponentKind = true;
	}
	
	struct Message
	{
		struct Init        : detail::MessageBase {} inline static constexpr INIT{};
		struct Cleanup     : detail::MessageBase {} inline static constexpr CLEANUP{};
		struct Enable      : detail::MessageBase {} inline static constexpr ENABLE{};
		struct Disable     : detail::MessageBase {} inline static constexpr DISABLE{};
		struct Update      : detail::MessageBase {} inline static constexpr UPDATE{};
		struct FixedUpdate : detail::MessageBase {} inline static constexpr FIXEDUPDATE{};
		struct Render      : detail::MessageBase {} inline static constexpr RENDER{};
	};

	class Object final
	{

		Object(Object* pParent, bool active);

		friend class scene::SceneManager;

	public:

		~Object();

		Object(Object&& other) = default;
		Object& operator = (Object&& other) = default;

		bool IsRoot() const noexcept;
		bool IsActive() const noexcept;

		// IsActive
		operator bool() const noexcept;

		// Set update state
		void SetState(bool active);

		// Add a component	
		template <detail::ComponentKind Data> requires std::movable<Data>
		Data& AddComponent(Data data);
		template <detail::ComponentKind Data, typename ... Arg> requires std::constructible_from<Data, Arg...>
		Data& AddComponent(Arg&& ...);
		template <detail::ComponentKind ... Data> requires (std::movable<Data> && ...)
		std::tuple<Data&...> AddComponents(Data ... data);

		// Get a component
		template <detail::ComponentKind Data>
		Data* HasComponent() const noexcept;
		template <detail::ComponentKind Data>
		Data& GetComponent() const;

		// Remove a component (throws if not present)
		template <detail::ComponentKind Data>
		auto RemoveComponent();

		// Add a child object
		Object& AddChild(bool active = true);
		// Remove a child object
		void RemoveChild(Object const& child);

		// Clear children and components
		void Clear();

		// Guaranteed, except for root (scene). Null reference for root. 
		Object& GetParent();
		Object const& GetParent() const;

		// View of all children
		auto GetAllChildren();
		auto GetAllChildren() const;

		// View of all active children
		auto GetActiveChildren();
		auto GetActiveChildren() const;

		// View of all inactive children
		auto GetInactiveChildren() const;

		void Do(Message::Init);
		void Do(Message::Cleanup);
		void Do(Message::Enable);
		void Do(Message::Disable);
		void Do(Message::Update);
		void Do(Message::FixedUpdate);
		void Do(Message::Render);

	private:


		Object(const Object& other) = delete;
		Object& operator = (const Object& other) = delete;

		class ComponentBase;

		template <typename Data>
		class Component;

		Object* m_pParent;
		std::vector<std::pair<std::type_index, std::unique_ptr<ComponentBase>>> m_Components;
		std::vector<std::unique_ptr<Object>> m_Children;

		bool m_Active;
		bool m_State;
		bool m_Initialised;
		bool m_Clear;

		void AddComponent(std::type_info const&, ComponentBase*);
		ComponentBase* GetComponent(std::type_info const&) const;
		std::unique_ptr<ComponentBase> ExtractComponent(std::type_info const&);

		static void ExceptComponentNotFound(std::type_info const&);

	};

	namespace detail
	{
		template <typename Message>
		concept MessageKind = std::derived_from<Message, MessageBase>
		&& requires (Object object, Message message)
		{ 
			object.Do(message); 
		};
	}

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

	template<detail::ComponentKind Data> requires std::movable<Data>
	inline Data& Object::AddComponent(Data data)
	{
		return AddComponent<Data, Data>(std::move(data));
	}

	template<detail::ComponentKind Data, typename ...Arg> requires std::constructible_from<Data, Arg...>
	inline Data& ecs::Object::AddComponent(Arg&& ... arg)
	{
		Component<Data>* pComponent{ new Component<Data>{ std::forward<Arg>(arg)... } };
		this->AddComponent(typeid(Data), pComponent);
		return *pComponent;
	}

	template<detail::ComponentKind ... Data> requires (std::movable<Data> && ...)
	inline std::tuple<Data&...> Object::AddComponents(Data ... data)
	{
		auto flatten = [&] (auto& self, auto first, auto ... rest)
		{
			auto out{ std::forward_as_tuple(this->Object::AddComponent<decltype(first), decltype(first)>(std::move(first))) };
			if constexpr (sizeof...(rest) == 0)
				return std::move(out);
			else
				return std::tuple_cat(std::move(out), self(self, std::move(rest)...));
		};
		return flatten(flatten, std::move(data)...);
	}

	template<detail::ComponentKind Data>
	inline Data* Object::HasComponent() const noexcept
	{
		if (auto pBase{ GetComponent(typeid(Data)) })
			return std::addressof(static_cast<Data&>(*static_cast<Component<Data>*>(pBase)));
		else
			return nullptr;
	}

	template<detail::ComponentKind Data>
	inline Data& Object::GetComponent() const
	{
		if (auto p{ HasComponent<Data>() })
			return *p;
		else
			return ExceptComponentNotFound(typeid(Data)), *p;
	}

	template<detail::ComponentKind Data>
	inline auto Object::RemoveComponent()
	{
		[[maybe_unused]] auto uptr{ ExtractComponent(typeid(Data)) };
		if constexpr (std::movable<Data>)
			return std::move(static_cast<Component<Data>&>(*uptr));
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

	class Object::ComponentBase
	{
	public:

		virtual ~ComponentBase() = default;

		virtual void Do(Message::Init, Object&) = 0;
		virtual void Do(Message::Cleanup, Object&) = 0;
		virtual void Do(Message::Enable, Object&) = 0;
		virtual void Do(Message::Disable, Object&) = 0;
		virtual void Do(Message::Update, Object&) = 0;
		virtual void Do(Message::FixedUpdate, Object&) = 0;
		virtual void Do(Message::Render, Object&) = 0;

		//virtual std::unique_ptr<ComponentBase> Clone() const = 0;

	protected:

		static void ExceptCannotCopy(std::type_info const&);

	};

}

namespace tei::internal::components { struct TextureRenderComponent; };

namespace tei::internal::ecs
{

	template <typename Data>
	class Object::Component : public Object::ComponentBase
	{
		Data m_Data;

	public:

		template <typename ... Arg>
		Component(Arg&& ... arg)
			: m_Data{ std::forward<Arg>(arg) ... }
		{}

		operator Data& () &
		{
			return m_Data;
		}
		operator Data const& () const
		{
			return m_Data;
		}
		operator Data&& () &&
		{
			return m_Data;
		}

	private:

		constexpr inline static std::true_type INTERNAL{};

		//virtual std::unique_ptr<ComponentBase> Clone() const override
		//{
		//	if constexpr (std::copyable<Data>)
		//		return std::unique_ptr<ComponentBase>{ new Component{ m_Data } };
		//	else
		//		ComponentBase::ExceptCannotCopy(typeid(Data));
		//	return {};
		//}

		virtual void Do(Message::Init, Object& parent) override
		{
			if constexpr (requires { ::OnInitialize(nullptr, m_Data, parent); })
				::OnInitialize(nullptr, m_Data, parent);
			else
			if constexpr (requires { ::OnInitialize(nullptr, m_Data); })
				::OnInitialize(nullptr, m_Data);

			if constexpr (requires { m_Data.OnInitialize(parent); })
				m_Data.OnInitialize(parent);
			else
			if constexpr (requires { m_Data.OnInitialize(); })
				m_Data.OnInitialize(); 
			else
			if constexpr (requires { ::OnInitialize(m_Data, parent); })
				::OnInitialize(m_Data, parent);
			else 
			if constexpr (requires { ::OnInitialize(m_Data); })
				::OnInitialize(m_Data); 
		}

		virtual void Do(Message::Cleanup, Object& parent) override
		{
			if constexpr (requires { ::OnCleanup(nullptr, m_Data, parent); })
				::OnCleanup(nullptr, m_Data, parent);
			else
			if constexpr (requires { ::OnCleanup(nullptr, m_Data); })
				::OnCleanup(nullptr, m_Data);

			if constexpr (requires { m_Data.OnCleanup(parent); })
				m_Data.OnCleanup(parent);
			else
			if constexpr (requires { m_Data.OnCleanup(); })
				m_Data.OnCleanup(); 
			else
			if constexpr (requires { ::OnCleanup(m_Data, parent); })
				::OnCleanup(m_Data, parent);
			else 
			if constexpr (requires { ::OnCleanup(m_Data); })
				::OnCleanup(m_Data); 
		}

		virtual void Do(Message::Enable, Object& parent) override
		{
			if constexpr (requires { ::OnEnable(nullptr, m_Data, parent); })
				::OnEnable(nullptr, m_Data, parent);
			else
			if constexpr (requires { ::OnEnable(nullptr, m_Data); })
				::OnEnable(nullptr, m_Data);

			if constexpr (requires { m_Data.OnEnable(parent); })
				m_Data.OnEnable(parent);
			else
			if constexpr (requires { m_Data.OnEnable(); })
				m_Data.OnEnable(); 
			else
			if constexpr (requires { ::OnEnable(m_Data, parent); })
				::OnEnable(m_Data, parent);
			else
			if constexpr (requires { ::OnEnable(m_Data, parent); })
				::OnEnable(m_Data, parent);
		}

		virtual void Do(Message::Disable, Object& parent) override
		{
			if constexpr (requires { ::OnDisable(nullptr, m_Data, parent); })
				::OnDisable(nullptr, m_Data, parent);
			else
			if constexpr (requires { ::OnDisable(nullptr, m_Data); })
				::OnDisable(nullptr, m_Data);

			if constexpr (requires { m_Data.OnDisable(parent); })
				m_Data.OnDisable(parent);
			else
			if constexpr (requires { m_Data.OnDisable(); })
				m_Data.OnDisable(); 
			else
			if constexpr (requires { ::OnDisable(m_Data, parent); })
				::OnDisable(m_Data, parent);
			else 
			if constexpr (requires { ::OnDisable(m_Data); })
				::OnDisable(m_Data); 
		}

		virtual void Do(Message::Update, Object& parent) override
		{
			if constexpr (requires { ::OnUpdate(nullptr, m_Data, parent); })
				::OnUpdate(nullptr, m_Data, parent);
			else
			if constexpr (requires { ::OnUpdate(nullptr, m_Data); })
				::OnUpdate(nullptr, m_Data);

			if constexpr (requires { m_Data.OnUpdate(parent); })
				m_Data.OnUpdate(parent);
			else
			if constexpr (requires { m_Data.OnUpdate(); })
				m_Data.OnUpdate(); 
			else
			if constexpr (requires { ::OnUpdate(m_Data, parent); })
				::OnUpdate(m_Data, parent);
			else 
			if constexpr (requires { ::OnUpdate(m_Data); })
				::OnUpdate(m_Data); 
		}

		virtual void Do(Message::FixedUpdate, Object& parent) override
		{
			if constexpr (requires { ::OnFixedUpdate(nullptr, m_Data, parent); })
				::OnFixedUpdate(nullptr, m_Data, parent);
			else
			if constexpr (requires { ::OnFixedUpdate(nullptr, m_Data); })
				::OnFixedUpdate(nullptr, m_Data);

			if constexpr (requires { m_Data.OnFixedUpdate(parent); })
				m_Data.OnFixedUpdate(parent);
			else
			if constexpr (requires { m_Data.OnFixedUpdate(); })
				m_Data.OnFixedUpdate(); 
			else
			if constexpr (requires { ::OnFixedUpdate(m_Data, parent); })
				::OnFixedUpdate(m_Data, parent);
			else 
			if constexpr (requires { ::OnFixedUpdate(m_Data); })
				::OnFixedUpdate(m_Data); 
		}

		virtual void Do(Message::Render, Object& parent) override
		{
			if constexpr (requires { ::OnRender(nullptr, m_Data, parent); })
				::OnRender(nullptr, m_Data, parent);
			else
			if constexpr (requires { ::OnRender(nullptr, m_Data); })
				::OnRender(nullptr, m_Data);

			if constexpr (requires { m_Data.OnRender(parent); })
				m_Data.OnRender(parent);
			else
			if constexpr (requires { m_Data.OnRender(); })
				m_Data.OnRender(); 
			else
			if constexpr (requires { ::OnRender(m_Data, parent); })
				::OnRender(m_Data, parent);
			else 
			if constexpr (requires { ::OnRender(m_Data); })
				::OnRender(m_Data); 
		}
	};

	inline auto Object::GetAllChildren()
	{
		namespace v = std::views;
		return v::transform(m_Children, utility::projectors::to_address<true>{});
	}
	inline auto Object::GetAllChildren() const
	{
		namespace v = std::views;
		return v::transform(m_Children, utility::projectors::to_address<true, true>{});
	}

	inline auto Object::GetActiveChildren()		
	{
		namespace v = std::views;
		return v::filter(GetAllChildren(), std::identity{});
	}
	inline auto Object::GetActiveChildren() const		
	{
		namespace v = std::views;
		return v::filter(GetAllChildren(), std::identity{});
	}

	inline auto Object::GetInactiveChildren() const		
	{
		namespace v = std::views;
		return m_Children | v::transform(utility::projectors::to_address<true, true>{}) | v::filter(std::logical_not{});
	}

}