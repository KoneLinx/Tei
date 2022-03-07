#pragma once

#include <tei/internal/utility.h>

namespace tei::internal::core
{

	class CoreFunction
	{

	public:

		void Run();
		void Stop();

	private:

		void GameLoop();
		void FrameUpdate();

		bool m_IsRunning{ true };

	};

	extern utility::Static<CoreFunction> Core;

}