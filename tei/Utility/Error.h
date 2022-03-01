#pragma once

#include <stdexcept>
#include <string>

#if defined(__cpp_lib_source_location)
#include <source_location>
#if defined(__cpp_lib_format)
#include <format>
#endif
#endif


namespace tei::internal::utility
{

	struct TeiRuntimeError : std::runtime_error
	{

		using source_location =
#if defined(__cpp_lib_source_location)
			std::source_location;
#else
			struct {};
#endif

		inline TeiRuntimeError(std::string_view message, source_location loc = {})
			: TeiRuntimeError{ message, char{}, loc }
		{}

		inline TeiRuntimeError(std::string_view message, auto data, source_location loc = {}) :
			std::runtime_error{
#if defined(__cpp_lib_format)
				std::format(
#if defined(DEBUG) || defined(_DEBUG)
					// Debug
					"[Error] {} ({}) {}(): {}\nData: {}",
					loc.file_name(),
					loc.line(),
					loc.function_name(),
					message,
					data
#else
					// Release
					"[Error] {}(): {}\nData: {}",
					loc.function_name(),
					message,
					data
#endif
				)
#else
				std::string{ "[Error] " }
#if defined(__cpp_lib_source_location)
#if defined(DEBUG) || defined(_DEBUG)
				+ loc.file_name() + " (" + std::to_string(loc.line()) + ") " + loc.function_name() + "(): "
				+ std::string{ message }
#else
				+ loc.function_name() + "(): "
				+ std::string{ message }
				+ std::to_string( data )
#endif
#else
				+ std::string{ (loc, message) }
				+ std::to_string( data )
#endif
#endif
			}
		{}
	};

}