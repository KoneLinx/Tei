#pragma once

#include "Chrono.h"

inline tei::internal::time::Clock::time_point tei::internal::time::Clock::now() noexcept
{
    return time_point{ duration{ Source::now() - m_Epoch } };
}

namespace tei::internal::time
{
    inline namespace literals
    {

        constexpr Clock::duration operator "" _d(unsigned long long d) noexcept
        {
            return Clock::duration{ d * (60.l * 60.l * 24.l) };
        }

        constexpr Clock::duration operator "" _h(unsigned long long d) noexcept
        {
            return Clock::duration{ d * (60.l * 60.l) };
        }

        constexpr Clock::duration operator "" _min(unsigned long long d) noexcept
        {
            return Clock::duration{ d / 60.l };
        }

        constexpr Clock::duration operator "" _s(unsigned long long d) noexcept
        {
            return Clock::duration{ d };
        }

        constexpr Clock::duration operator "" _ms(unsigned long long d) noexcept
        {
            return Clock::duration{ d / 1e3l };
        }

        constexpr Clock::duration operator "" _us(unsigned long long d) noexcept
        {
            return Clock::duration{ d / 1e6l };
        }

        constexpr Clock::duration operator "" _ns(unsigned long long d) noexcept
        {
            return Clock::duration{ d / 1e9l };
        }

        constexpr Clock::duration operator "" _d(long double d) noexcept
        {
            return Clock::duration{ d * (60.l * 60.l * 24.l) };
        }

        constexpr Clock::duration operator "" _h(long double d) noexcept
        {
            return Clock::duration{ d * (60.l * 60.l) };
        }

        constexpr Clock::duration operator "" _min(long double d) noexcept
        {
            return Clock::duration{ d / 60.l };
        }

        constexpr Clock::duration operator "" _s(long double d) noexcept
        {
            return Clock::duration{ d };
        }

        constexpr Clock::duration operator "" _ms(long double d) noexcept
        {
            return Clock::duration{ d / 1e3l };
        }

        constexpr Clock::duration operator "" _us(long double d) noexcept
        {
            return Clock::duration{ d / 1e6l };
        }

        constexpr Clock::duration operator "" _ns(long double d) noexcept
        {
            return Clock::duration{ d / 1e9l };
        }

    }
}