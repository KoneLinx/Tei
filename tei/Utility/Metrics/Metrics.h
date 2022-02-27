#pragma once

//#define METRICS

#ifndef METRICS
#if (defined(__cpp_lib_source_location)) && (defined(DEBUG) || defined(_DEBUG))
#define METRICS
#endif
#endif

#ifdef METRICS

#include <chrono>
#include <thread>
#include <variant>
#include <fstream>
#include <list>
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
		static BlockTimer TimeBlock (source_location src);

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
#define METRICS_TIMEBLOCK    auto _blocktimer = metrics::MetricLog<>::TimeBlock(std::source_location::current());
#define METRICS_INITLOG(...) metrics::MetricLog<> _metriclog{ new std::ofstream{ __VA_ARGS__ } };

}

#else

#define METRICS_REGISTER    
#define METRICS_TIMEBLOCK   
#define METRICS_INITLOG(...)

#endif