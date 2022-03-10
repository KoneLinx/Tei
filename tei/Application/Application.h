#pragma once

#include <vector>
#include <utility>
#include <string>
#include <span>

#include <tei/internal/utility.h>

namespace tei::internal::application
{

	class Application
	{

		explicit Application(int, char const* const*);

	public:

		friend Application& StartApplication(int, char const* const*);

		~Application();

		inline auto const& GetWindow() const
		{ return m_Window; }

		void SetFullscreen(bool state, bool fake = false);
		void SetWindowSize(int width, int height);
		void SetWindowBorder(bool state);

		inline std::span<std::string_view const> Args() const
		{ return m_Args; }

		void Quit() const;

		struct Window
		{
			void* pData;
			int x, y;
			int w, h;
		};

	private:

		void OpenWindow();
		void CloseWindow();

		void InitAudio();
		void ExitAudio();

		std::vector<std::string_view> const m_Args{};
		Window m_Window{};

	public:

		using Service = utility::Service<Application>;

	};

	Application& StartApplication(int argc = {}, char const* const* argv = {});
	void StopApplication();

	extern Application::Service ApplicationService;

}

namespace tei::external
{

	static constexpr auto const& Application { tei::internal::application::ApplicationService };

}

