#pragma once

#ifndef METRICS
#define METRICS 1
#endif


#ifndef METRICS
#if (defined(__cpp_lib_source_location)) && (defined(DEBUG) || defined(_DEBUG))
#define METRICS
#endif
#else
#if (2 * METRICS +1 == 1)
#undef METRICS
#endif
#endif

#include <chrono>
#include <thread>
#include <variant>
#include <list>

#ifdef METRICS

#include <fstream>
#include <mutex>

#include <source_location>

namespace metrics
{

	using Clock = std::chrono::steady_clock;

	template <typename = void>
	class MetricLog
	{

		using source_location = std::source_location;

		class BlockTimer;

		class ThreadLog;

	public:

		MetricLog(std::ostream* stream);
		~MetricLog();
		
		static void Register(source_location src);
		static BlockTimer TimeBlock (source_location src, std::string_view pretty_fn);

	private:

		class BlockTimer final
		{
		public:
			~BlockTimer();
		};

		struct Event
		{
			Clock::time_point time;
			std::thread::id id;
			source_location src;
			std::string_view extra{};
		};
		struct Begin : Event
		{};
		struct End : Event
		{};
		using Data = std::variant<Event, Begin, End>;

		class ThreadLog
		{
		public:

			ThreadLog(std::list<Data>& mainLog, std::mutex& mutex)
				: m_Mutex{ mutex }
				, m_MainLog{ mainLog }
			{}

			~ThreadLog();

			std::list<Data>* operator -> ();
			void Clear();

		private:

			std::list<Data> m_Log{};
			std::reference_wrapper<std::mutex> m_Mutex;
			std::reference_wrapper<std::list<Data>> m_MainLog;
		};

		static void Print(Event const& data);
		static void Print(Begin const& data);
		static void Print(End   const& data);

		static std::unique_ptr<std::ostream> m_Stream;

		static std::list<Data> m_MainLog;
		static std::mutex m_Mutex;
		static thread_local ThreadLog m_ThreadLog;

	};

#define METRICS_REGISTER     metrics::MetricLog<>::Register(METRICS_SOURCE_LOCATION);
#define METRICS_TIMEBLOCK    auto _blocktimer = metrics::MetricLog<>::TimeBlock(std::source_location::current(), __FUNCSIG__);
#define METRICS_INITLOG(...) metrics::MetricLog<> _metriclog{ new std::ofstream{ __VA_ARGS__ } };

}

#else

#define METRICS_REGISTER    
#define METRICS_TIMEBLOCK   
#define METRICS_INITLOG(...)

#endif