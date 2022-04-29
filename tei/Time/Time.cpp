#include "teipch.h"
#include "Time.h"

namespace tei::internal::time
{
	utility::Static<TimeOject const> Time{
		TimeOject{
			.global = {},
			.frame = {
				.step = 1_s / 180,
				.fixed = false,
				.allow_drop = true
			},
			.fixed = {
				.step = 1_s / 20,
				.fixed = true,
				.allow_drop = false,
			}
		}
	};

	thread_local TimeOject::Thread const* TimeOject::thread{};

}