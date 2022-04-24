#pragma once

#include "Time.h"

namespace tei::internal::time
{

	struct TimeEvery
	{

	public:

		TimeEvery(Clock::duration interval, Clock::time_point start = Time->thread->now)
			: m_Interval{ interval }
			, m_Current{}
			, m_Next{ start + interval }
			, m_Active{ true }
		{}

		operator bool()
		{
			if (m_Current != std::exchange(m_Current, Time->thread->now))
			{
				if (m_Next - m_Current <= 0_s)
				{
					m_Next = m_Current + m_Interval;
					return (m_Active = true);
				}
				else return (m_Active = false);
			}
			else return m_Active;
		}

	private:

		time::Clock::duration m_Interval;
		time::Clock::time_point m_Current;
		time::Clock::time_point m_Next;
		bool m_Active;

	};

	struct TimeOnce
	{

	public:

		TimeOnce(Clock::time_point after = Time->thread->now)
			: m_Current{ time::Time->thread->now }
			, m_Time{ after }
			, m_Active{ true }
		{}

		operator bool()
		{
			if (m_Active <= 1)
			{
				if (m_Current != std::exchange(m_Current, Time->thread->now))
				{
					return ++m_Active == 1;
				}
				else return m_Active == 1;
			}
			else return false;
		}

	private:

		time::Clock::time_point m_Current;
		time::Clock::time_point m_Time;
		char m_Active;

	};

	struct TimeTimes
	{

	public:

		TimeTimes(Clock::duration interval, size_t count, Clock::time_point start = Time->thread->now)
			: m_Interval{ interval }
			, m_Current{}
			, m_Next{ start + interval }
			, m_Count{ count }
			, m_Active{ true }
		{}

		operator bool()
		{
			if (m_Active || m_Count > 0)
			{
				if (m_Current != std::exchange(m_Current, Time->thread->now))
				{
					if (m_Next - m_Current <= 0_s)
					{
						--m_Count;
						m_Next = m_Current + m_Interval;
						return (m_Active = true);
					}
					else return (m_Active = false);
				}
				else return m_Active;
			}
			else return false;
		}

	private:

		time::Clock::duration m_Interval;
		time::Clock::time_point m_Current;
		time::Clock::time_point m_Next;
		size_t m_Count;
		bool m_Active;

	};

}

namespace tei::external::time
{
	using tei::internal::time::TimeEvery;
	using tei::internal::time::TimeOnce;
	using tei::internal::time::TimeTimes;
}