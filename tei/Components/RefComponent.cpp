#include "teipch.h"

#include "RefComponent.h"

void tei::internal::components::detail::ExceptRefNotValid(std::type_info const& type)
{
	throw tei::internal::utility::TeiRuntimeError{ "Cannot find or create required component", type.name() };
}
