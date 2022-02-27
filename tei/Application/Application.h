#pragma once

#include <vector>
#include <utility>
#include <string>
#include <span>

#include <tei/utility.h>

namespace tei::internal::application
{

	struct Window
	{
		void* pData;
		int x, y;
		int w, h;
	};

	class Application
	{

		explicit Application(int, char const* const*);

	public:

		friend Application& StartApplication(int, char const* const*);

		~Application();

		inline Window const& Window() const
		{ return m_Window; }

		inline std::span<std::string_view const> Args() const
		{ return m_Args; }

		void Quit() const;

	private:

		void OpenWindow();
		void CloseWindow();

		void InitAudio();
		void ExitAudio();

		std::vector<std::string_view> const m_Args{};
		struct Window m_Window{};

	public:

		using Service = utility::Service<Application const>;

	};

	Application& StartApplication(int argc = {}, char const* const* argv = {});
	void StopApplication();

	extern Application::Service ApplicationService;

}

namespace tei::external
{

	static constexpr auto const& Application { tei::internal::application::ApplicationService };

}

