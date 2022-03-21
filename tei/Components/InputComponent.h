#pragma once

#include <unordered_map>
#include <typeindex>
#include <tei/input.h>
#include <tei/internal/ecs.h>

namespace tei::internal::input::detail
{

	class InputComponent
	{
	public:

		template <typename InputType>
		Command<InputType>& AddCommand(Command<InputType> command = {})
		{
			auto& data = input::Input->AddCommand(std::move(command));
			m_Commands.push_back(data);
			return data;
		}

		template <typename InputType, typename Action> requires std::constructible_from<Command<InputType>, InputType, Action>
		Command<InputType>& AddCommand(InputType input, Action action)
		{
			auto& command = input::Input->AddCommand(std::move(input), std::move(action));
			m_Commands.push_back(command);
			return command;
		}

		void Clear();

	private:

		std::vector<utility::AnyRef> m_Commands{};

	};

}

namespace tei::internal::components
{
	using tei::internal::input::detail::InputComponent;
}

namespace tei::external::components
{
	using tei::internal::components::InputComponent;
}

void OnDisable(tei::internal::Internal, tei::internal::input::detail::InputComponent&);
