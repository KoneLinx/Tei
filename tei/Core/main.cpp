
#include "Core.h"

#include <SDL.h>

#include <tei/application.h>
#include <tei/internal/Utility/Error.h>

#if defined(DEBUG) || defined(_DEBUG)
#include <vld.h>
#endif

int main(int argc, char** argv)
{
	try
	{
		{

			//METRICS_INITLOG("_metrics.json");
			METRICS_TIMEBLOCK;

			//puts("Tei engine - starting");

			srand(unsigned(time(nullptr)));
			tei::internal::application::StartApplication(argc, argv);
			tei::internal::core::Core->Run();
			tei::internal::application::StopApplication();

			//puts("Tei engine - quit successfully");

		}
		return 0;
	}
	catch (const tei::internal::utility::TeiRuntimeError&)
	{
		fputs("[FATAL] Uncaught engine exception!", stderr);
		fputc('\n', stderr);
	}
	catch (const std::exception& e)
	{
		fputs("[FATAL] Uncaught exception!", stderr);
		fputc('\n', stderr);
		fputs(e.what(), stderr);
		fputc('\n', stderr);
	}
	catch (...)
	{
		fputs("[FATAL] Uncaught exception!", stderr);
		fputc('\n', stderr);
		fputs("Error unknown", stderr);
		fputc('\n', stderr);
	}
	return -1;
}