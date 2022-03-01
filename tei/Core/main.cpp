
#include "Core.h"

#include <SDL.h>

#include <tei/application.h>

#if defined(DEBUG) || defined(_DEBUG)
#include <vld.h>
#endif

int _main(int argc, char** argv)
{
	try
	{
		{

			METRICS_INITLOG("_metrics.json");
			METRICS_TIMEBLOCK;

			puts("Tei engine - starting");


			srand(unsigned(time(nullptr)));
			tei::internal::application::StartApplication(argc, argv);
			tei::internal::core::Core{}.Run();
			tei::internal::application::StopApplication();

			puts("Tei engine - quit successfully");

		}
		return 0;
	}
	catch (const std::exception& e)
	{
		puts("[Error] Uncaught exception!");
		puts(e.what());
		puts("[Error] Quitting...");
	}
	catch (...)
	{
		puts("[Error] Uncaught exception!");
		puts("[Error] exception Unknown");
		puts("[Error] Quitting...");
	}
	return -1;
}