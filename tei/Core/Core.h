#pragma once

#include <tei/utility.h>

namespace tei::internal::core
{

	class CoreFunction
	{

	public:

		void Run();
		void Stop();

	private:

		bool* m_IsRunning{};

	};

	extern utility::Static<CoreFunction> Core;

}