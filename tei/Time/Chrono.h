#pragma once

#include <chrono>
#include <type_traits>

#include "Clock.h"

template <typename Other>
struct std::common_type<Other, tei::internal::time::TimeRep> : std::common_type<Other, tei::internal::time::TimeRep::Rep>
{};
template <typename Other>
struct std::common_type<tei::internal::time::TimeRep, Other> : std::common_type<tei::internal::time::TimeRep::Rep, Other>
{};
template <>
struct std::common_type<tei::internal::time::TimeRep, tei::internal::time::TimeRep> : std::common_type<tei::internal::time::TimeRep::Rep, tei::internal::time::TimeRep::Rep>
{};

template <>
class std::chrono::duration<tei::internal::time::TimeRep, std::ratio<1>>
{
public:

    using rep = tei::internal::time::TimeRep;
    using period = std::ratio<1>;

    constexpr duration() = default;

    template <std::convertible_to<rep> Rep2>
    constexpr duration(Rep2 const& value) noexcept(std::is_nothrow_convertible_v<Rep2, rep>)
        : m_Value{ static_cast<rep::Rep>(value) }
    {}

    template <std::convertible_to<rep> Rep2, class Period2>
    constexpr duration(duration<Rep2, Period2> const& other) requires std::constructible_from<std::chrono::duration<rep::Rep, period>, decltype(other)>
        : m_Value(std::chrono::duration<rep::Rep, period>{ other }.count())
    {}

    [[nodiscard]] constexpr typename rep::Rep count() const noexcept
    {
        return m_Value;
    }

    [[nodiscard]] constexpr duration operator+() const noexcept
    {
        return *this;
    }
    [[nodiscard]] constexpr duration operator-() const noexcept
    {
        return duration{ -m_Value };
    }

    constexpr duration& operator ++ () noexcept
    {
        ++static_cast<rep::Rep&>(m_Value);
        return *this;
    }
    [[nodiscard]] constexpr duration operator ++ (int) noexcept
    {
        return duration{ static_cast<rep::Rep&>(m_Value)++ };
    }

    constexpr duration& operator -- () noexcept
    {
        --static_cast<rep::Rep&>(m_Value);
        return *this;
    }
    [[nodiscard]] constexpr duration operator -- (int) noexcept
    {
        return duration{ static_cast<rep::Rep&>(m_Value)-- };
    }

    constexpr duration& operator += (const duration& other)  noexcept
    {
        m_Value += other.count();
        return *this;
    }
    constexpr duration& operator -= (const duration& other) noexcept
    {
        m_Value -= other.count();
        return *this;
    }

    constexpr duration& operator *= (const rep& other) noexcept
    {
        m_Value *= other;
        return *this;
    }
    constexpr duration& operator /= (const rep& other) noexcept
    {
        m_Value /= other;
        return *this;
    }

    constexpr duration& operator %= (const rep& other) noexcept
    {
        m_Value %= other;
        return *this;
    }
    constexpr duration& operator %= (const duration& other) noexcept
    {
        m_Value %= other.count();
        return *this;
    }

    [[nodiscard]] static constexpr duration zero() noexcept
    {
        return {};
    }
    [[nodiscard]] static constexpr duration min() noexcept
    {
        return duration{ std::numeric_limits<rep>::min() };
    }
    [[nodiscard]] static constexpr duration max() noexcept
    {
        return duration{ std::numeric_limits<rep>::max() };
    }

    // Extra

    constexpr operator rep() const noexcept
    {
        return m_Value;
    }
    constexpr operator rep::Rep() const noexcept
    {
        return static_cast<rep::Rep>(m_Value);
    }
    template <typename T> requires std::convertible_to<rep::Rep, T>
    constexpr operator T() const noexcept
    {
        return static_cast<T>(m_Value);
    }

private:

    rep m_Value{};

};

template <>
class std::chrono::time_point<tei::internal::time::Clock, std::chrono::duration<tei::internal::time::TimeRep, std::ratio<1>>> : public std::chrono::duration<tei::internal::time::Clock::rep, std::ratio<1>>
{
    using super = std::chrono::duration<tei::internal::time::Clock::rep, std::ratio<1>>;
public:

    using clock = tei::internal::time::Clock;
    using duration = std::chrono::duration<tei::internal::time::TimeRep, std::ratio<1>>;
    using rep = duration::rep;
    using period = duration::period;

    constexpr time_point() = default;

    constexpr time_point(const duration& duration) noexcept
        : super{ duration }
    {}

    template <std::convertible_to<duration> Duration2>
    constexpr time_point(const time_point<clock, Duration2>& other) noexcept(std::is_nothrow_convertible_v<Duration2, duration>)
        : super{ other.time_since_epoch() }
    {}

    [[nodiscard]] constexpr duration time_since_epoch() const noexcept
    {
        return *this;
    }

    constexpr time_point& operator += (duration const& other) noexcept
    {
        super::operator+=(other);
        return *this;
    }
    constexpr time_point& operator -= (duration const& other) noexcept
    {
        super::operator-=(other);
        return *this;
    }

    [[nodiscard]] static constexpr time_point min() noexcept 
    {
        return time_point(duration::min());
    }
    [[nodiscard]] static constexpr time_point max() noexcept
    {
        return time_point(duration::max());
    }

    // EXTRA

    template <typename Clock, typename Duration> //requires std::convertible_to<clock::Source::time_point, std::chrono::time_point<Clock, Duration>>
    [[nodiscard]] operator std::chrono::time_point<Clock, Duration> () const
    {
        return std::chrono::time_point<Clock, Duration>{ std::chrono::duration_cast<Duration>(std::chrono::clock_cast<Clock>(clock::epoch()).time_since_epoch()) + std::chrono::duration_cast<Duration>(this->time_since_epoch()) };
    }

    //template <typename Rep, typename Period> //requires std::convertible_to<clock::Source::time_point::duration, std::chrono::duration<Rep, Period>>
    //[[nodiscard]] operator std::chrono::duration<Rep, Period> () const
    //{
    //    return std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(clock::epoch().time_since_epoch()) + std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(this->time_since_epoch());
    //}

    //constexpr auto operator <=> (clock::duration const& duration) const
    //{
    //    return time_since_epoch() <=> duration;
    //}
    
private:

    //duration m_Value{};

};