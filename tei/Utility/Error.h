#pragma once

#include <stdexcept>
#include <string>
#include <sstream>

#if defined(__cpp_lib_source_location) && (defined(DEBUG) || defined(_DEBUG))
#include <source_location>
#if defined(__cpp_lib_format) && 0
#include <format>
#endif
#endif

#if defined(DEBUG) || defined(_DEBUG)
#include <cassert>
#endif

#include <iostream>

namespace tei::internal::utility
{

	struct TeiRuntimeError : std::runtime_error
	{

		using source_location =
#if defined(__cpp_lib_source_location) && (defined(DEBUG) || defined(_DEBUG))
			std::source_location;
#else
			struct {};
#endif

		inline TeiRuntimeError(std::string_view message, source_location loc = {})
			: TeiRuntimeError{ message, char{'-'}, loc}
		{}

		inline TeiRuntimeError(std::string_view message, auto data, source_location loc = {}) :
			std::runtime_error{
#if defined(__cpp_lib_format) && 0
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
				(loc, std::stringstream{}
					<< "[Error] "
#if defined(__cpp_lib_source_location) && (defined(DEBUG) || defined(_DEBUG))
#if defined(DEBUG) || defined(_DEBUG)
					<< loc.file_name() << " (" << std::to_string(loc.line()) << ") " << loc.function_name() << "(): "
#else 
					<< loc.function_name() << "(): "
#endif
#endif
					<< message
					<< "\nData: " << data
#endif
				).str()
			}
		{
			std::cerr << what() << std::endl;
#if defined(DEBUG) || defined(_DEBUG)
			std::cerr << "[DEBUG] Halt? [Y/n] ";
			fflush(stdin);
			if(char(getc(stdin)) != 'n')
				abort();
			std::cerr << "[DEBUG] Resuming" << std::endl;
#endif
		}
	};

}