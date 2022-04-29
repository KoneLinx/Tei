#include "teipch.h"
#include "Command.h"

void tei::internal::input::detail::CommandNoUserDataError()
{
	throw utility::TeiRuntimeError{ "Command has no user data" };
}
