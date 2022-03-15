
#pragma once

#include <vector>
#include <memory>

#include "InputType.h"
#include "Command.h"

#include <tei/internal/utility.h>

namespace tei::internal::input
{

	class InputManager
	{
	public:

		InputManager();
		~InputManager();

		template <typename InputType>
		Command<InputType>& AddCommand(Command<InputType> command = {});
		
		template <typename InputType, typename Action> requires std::constructible_from<Command<InputType>, InputType, Action>
		Command<InputType>& AddCommand(InputType input, Action action);

		void ProcessInput();

		bool IsPressed(InputBinary button) const;

		template <typename InputType>
		typename InputType::Data GetInputData(InputType input) const;

	private:

		SomeCommonInputData GetInputDataImpl(SomeCommonInputType) const;

		std::tuple<
			std::vector<std::unique_ptr<CommandBinary>>,
			std::vector<std::unique_ptr<CommandAnalog>>,
			std::vector<std::unique_ptr<CommandAnalog2>>
		> m_Commands;

		struct PollData;
		std::unique_ptr<PollData> m_PollData;

	public:

		using Service = utility::Service<InputManager>;

	};

	extern InputManager::Service Input;

	template <typename InputType>
	inline Command<InputType>& InputManager::AddCommand(Command<InputType> command)
	{
		auto* pCommand{ new Command<InputType>{ std::move(command) } };
		std::get<std::vector<std::unique_ptr<Command<InputType>>>>(m_Commands).emplace_back(pCommand);
		return *pCommand;
	}

	template <typename InputType, typename Action> requires std::constructible_from<Command<InputType>, InputType, Action>
	inline Command<InputType>& InputManager::AddCommand(InputType input, Action action)
	{
		return AddCommand(Command<InputType>{ input, std::move(action) });
	}

	template<typename InputType>
	inline typename InputType::Data InputManager::GetInputData(InputType input) const
	{
		return std::get<typename InputType::Data>(GetInputDataImpl(std::move(input)));
	}

}

namespace tei::external
{
	constexpr static auto const& Input{ tei::internal::input::Input };
}