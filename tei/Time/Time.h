#pragma once

#include "Clock.impl.h"
#include <tei/internal/utility.h>

namespace tei::internal::time
{

	struct TimeOject
	{

		struct
		{
			Clock::time_point           now          {};
			Clock::time_point const	    startup      {};
		}
		global;

		struct
		{
			Clock::time_point			now          {};
			Clock::duration				delta        {};
			Clock::duration				lag          {};

			Clock::duration				step         { 1_s / 60 };
			bool						fixed        { false };
			bool						vsync        {};
		}
		frame;

	};

	extern utility::Static<TimeOject const> Time;

}

namespace tei::external
{
	constexpr static auto const& Time{ ::tei::internal::time::Time };
}
