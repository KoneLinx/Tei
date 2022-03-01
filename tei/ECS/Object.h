#pragma once

#include <vector>
#include <memory>
#include <ranges>
#include <array>
#include <span>

#include <tei/utility.h>

namespace tei::internal::ecs
{

	enum struct Message
	{
		INIT,
		FREE,
		ENABLE,
		DISABLE,
		UPDATE,
		RENDER,
		_COUNT
	};

	template <Message, typename Data>
	void On(Data)
	{}

	class Object final
	{
	public:

		Object(bool active);

		~Object();

		Object(const Object& other) = delete;
		Object& operator = (const Object& other) = delete;

		Object(Object&& other) = default;
		Object& operator = (Object&& other) = default;

		operator bool() const noexcept;

		void Activate(bool state) noexcept;

		template <typename Data>
		Data& AddComponent(Data data = {});

		template <typename Data>
		Data& GetComponent() const noexcept;

		Object& AddChild(bool active = true);

		auto AllChildren() const noexcept;
		auto ActiveChildren() const noexcept;
		auto InactiveChildren() const noexcept;

		void Do(Message);

	private:

		template <typename Data = void>
		struct Component final : Component<>
		{
			Data data;

			Component(Data data);

			static void Handle(Component<>& data, Message message);
		};

		template <>
		struct Component<void>
		{
			virtual ~Component() = default;

			using Handle = void(*)(Component<>&, Message);
		};


		void AddComponent(Component<>*, Component<>::Handle);
		Component<>& GetComponent(Component<>::Handle) const;

		std::vector<std::pair<std::unique_ptr<Component<>>, Component<>::Handle>> m_Components;

		std::vector<Object> m_Children;

		bool m_Active;
		bool m_SetActive;
		bool m_Initialised;

	};

}

using tei::internal::ecs::On;

namespace tei::external
{
	namespace ecs = tei::internal::ecs;
}

namespace tei::internal::ecs
{

	inline Object::operator bool() const noexcept
	{
		return m_Active;
	}

	template<typename Data>
	inline Data& Object::AddComponent(Data data)
	{
		Component<Data>* pComponent{ new Component<Data>{ std::move(data) } };
		this->AddComponent(pComponent, &Component<Data>::Handle);
		return pComponent->data;
	}

	template<typename Data>
	inline Data& Object::GetComponent() const noexcept
	{
		return static_cast<Component<Data>&>(*std::get<0>(*std::ranges::find(m_Components, &Component<Data>::Handle, utility::tuple_index_projector<1>{}))).data;
	}
	
	inline auto Object::AllChildren() const noexcept
	{
		return std::span{ m_Children };
	}
	
	inline auto Object::ActiveChildren() const noexcept
	{
		return std::views::filter(this->AllChildren(), std::identity{});
	}
	
	inline auto Object::InactiveChildren() const noexcept
	{
		return std::views::filter(this->AllChildren(), std::logical_not{});
	}

	template<typename Data>
	inline Object::Component<Data>::Component(Data data)
		: data{ std::move(data) }
	{}

	template<typename Data>
	inline void Object::Component<Data>::Handle(Component<>& data, Message message)
	{
		[] <size_t ... MESSAGE> (Component& data, Message message, std::index_sequence<MESSAGE...>)
		{
			((message == static_cast<Message>(MESSAGE) && (On<static_cast<Message>(MESSAGE)>(data.data), true)) || ...);
		}
		(static_cast<Component&>(data), message, std::make_index_sequence<static_cast<size_t>(Message::_COUNT)>{});
	}

}
