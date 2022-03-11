#pragma once

#include <utility>
#include <cmath>
#include <chrono>

namespace tei::internal::time
{

    class TimeRep
    {
    public:

        using Rep = float;

        constexpr TimeRep(Rep value = {}) noexcept
            : m_Value{ value }
        {}

        constexpr operator Rep& () & noexcept
        {
            return m_Value;
        }
        constexpr operator Rep () const noexcept
        {
            return m_Value;
        }

        constexpr TimeRep operator % (Rep modulus) const noexcept
        {
            if (std::is_constant_evaluated())
            {
                return m_Value - Rep(long long(m_Value / modulus)) * modulus;
            }
            else return std::fmod(m_Value, modulus);
        }
        constexpr TimeRep& operator %= (Rep modulus) noexcept
        {
            return *this = this->operator%(modulus);
        }

    private:

        Rep m_Value;
    };

    class Clock
    {

        using Source = std::chrono::system_clock;

    public:

        using rep = tei::internal::time::TimeRep;
        using period = std::ratio<1>;
        using duration = std::chrono::duration<rep, period>;
        using time_point = std::chrono::time_point<Clock>;

        static constexpr bool is_steady = Source::is_steady;

        [[nodiscard]] static inline time_point now() noexcept;

    private:

        friend time_point;
        static Source::time_point epoch() noexcept
        {
            return m_Epoch;
        }

        static Source::time_point const m_Epoch;

    };

    inline namespace literals 
    {}

}

namespace tei::external
{
    using Clock = tei::internal::time::Clock;

    namespace literals
    {
        using namespace tei::internal::time::literals;
    }
}
