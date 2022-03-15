#pragma once

#include "Clock.impl.h"
#include <tei/internal/utility.h>

namespace tei::internal::time
{

	struct TimeOject
	{

		struct Global
		{
			Clock::time_point now;
			Clock::time_point startup;
		}
		global;

		struct Thread
		{
			Clock::time_point now;
			Clock::duration	delta;
			Clock::duration	lag;

			Clock::duration	step;
			bool fixed;
			bool allow_drop;
		}
		frame, fixed;

		static thread_local Thread const* thread;

	};

	extern utility::Static<TimeOject const> Time;

}

namespace tei::external
{
	inline constexpr auto const& Time{ ::tei::internal::time::Time };
}
