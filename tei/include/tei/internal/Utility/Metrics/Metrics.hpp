#pragma once

#include "Metrics.h"

#ifdef METRICS

namespace metrics
{

	template <typename T>
	std::unique_ptr<std::ostream> MetricLog<T>::m_Stream{};

	template <typename T>
	std::list<typename MetricLog<T>::Data> MetricLog<T>::m_MainLog{};
	
	template <typename T>
	std::mutex MetricLog<T>::m_Mutex{};

	template <typename T>
	thread_local MetricLog<T>::ThreadLog MetricLog<T>::m_ThreadLog{ MetricLog<>::m_MainLog, MetricLog<>::m_Mutex };

	template <typename T>
	void MetricLog<T>::ThreadLog::Clear()
	{
		if (!m_Log.empty())
		{
			m_Mutex.get().lock();
			m_MainLog.get().splice(m_MainLog.get().end(), m_Log);
			m_Mutex.get().unlock();
		}
	}
	template <typename T>
	MetricLog<T>::ThreadLog::~ThreadLog()
	{
		Clear();
	}

	template <typename T>
	std::list<typename MetricLog<T>::Data>* MetricLog<T>::ThreadLog::operator->()
	{
		 return &m_Log;
	}

	template <typename T>
	inline MetricLog<T>::~MetricLog()
	{
		*m_Stream << '[';
		Print(Begin{ Clock::now(), std::this_thread::get_id(), std::source_location::current() });

		m_ThreadLog.Clear();

		for (Data const& data : m_MainLog)
			std::visit([](auto const& data) { Print(data); }, data);

		Print(End{ Clock::now(), std::this_thread::get_id() });
		*m_Stream << "{}]";
		m_Stream.reset();
	}

	template <typename T>
	inline MetricLog<T>::BlockTimer::~BlockTimer()
	{
		if (m_Stream)
			m_ThreadLog->push_back(End{Clock::now(), std::this_thread::get_id()});
	}

	template <typename T>
	inline void MetricLog<T>::Register(source_location src)
	{
		if (m_Stream)
			m_ThreadLog->push_back(Event{ Clock::now(), std::this_thread::get_id(), src });
	}

	template <typename T>
	MetricLog<T>::BlockTimer MetricLog<T>::TimeBlock(source_location src, std::string_view pretty_fn)
	{
		if (m_Stream)
			m_ThreadLog->push_back(Begin{ Clock::now(), std::this_thread::get_id(), src, pretty_fn });
		return {};
	}

	template <typename T>
	inline MetricLog<T>::MetricLog(std::ostream* stream)
	{
		m_Stream = std::unique_ptr<std::ostream>{ stream };
	}

	template <typename T>
	inline void MetricLog<T>::Print(Event const& data)
	{
		char buffer[4096]{};
		int pos{};
		auto time = data.time.time_since_epoch().count() / 1000;
		auto filename = strrchr(data.src.file_name(), '\\') + 1;
		pos += sprintf_s(
			buffer,
R"({ 
	"name":"%s",
	"ph":"X",
	"ts":%lld,
	"dur":1,
	"pid":0,
	"tid":%u,
	"args":
	{
		"file_name":"%s",
		"function_name":"%s",
		"line":%d
	}
},)",
			data.extra.empty() ? data.src.function_name() : data.extra.data(),
			time,
			std::bit_cast<uint32_t>(data.id),
			filename,
			data.src.function_name(),
			data.src.line()
		);
		*m_Stream << std::string_view{ buffer, size_t(pos) };
	}

	template <typename T>
	inline void MetricLog<T>::Print(Begin const& data)
	{
		char buffer[4096]{};
		int pos{};
		auto time = data.time.time_since_epoch().count() / 1000;
		auto filename = strrchr(data.src.file_name(), '\\') + 1;
		pos += sprintf_s(
			buffer,
R"({
	"name":"%s",
	"cat":"",
	"ph":"B",
	"ts":"%lld",
	"pid":0,
	"tid":%u,
	"args":
	{
		"file_name":"%s",
		"function_name":"%s",
		"line":%u
	}
},)",
			data.extra.empty() ? data.src.function_name() : data.extra.data(),
			time,
			std::bit_cast<uint32_t>(data.id),
			filename,
			data.src.function_name(),
			data.src.line()
		);
		*m_Stream << std::string_view{ buffer, size_t(pos) };
	}

	template <typename T>
	inline void MetricLog<T>::Print(End const& data)
	{
		char buffer[4096]{};
		int pos{};
		auto now = data.time.time_since_epoch().count() / 1000;
		pos += sprintf_s(
			buffer,
R"({
	"ph":"E",
	"ts":"%lld",
	"pid":0,
	"tid":%u,
	"args":{}
},)",
			now,
			std::bit_cast<uint32_t>(data.id)
		);
		*m_Stream << std::string_view{ buffer, size_t(pos) };
	}

}

#endif