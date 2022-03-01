#pragma once

namespace tei::internal::core
{

	class Core
	{

	public:

		void Run();
		void Stop();

	private:

		void GameLoop();
		void FrameUpdate();

		bool m_IsRunning{ true };

	};

}