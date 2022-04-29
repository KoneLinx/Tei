#include "teipch.h"
#include "Projector_as_const.h"

using tei::internal::utility::projectors::as_const;
using std::is_same_v;

template<typename T>
using as_const_t = decltype(as_const{}(std::declval<T>()));

static_assert(is_same_v< as_const_t< int                  >, int const &&        >);
static_assert(is_same_v< as_const_t< int const            >, int const &&        >);
static_assert(is_same_v< as_const_t< int       &&         >, int const &&        >);
static_assert(is_same_v< as_const_t< int const &&         >, int const &&        >);
static_assert(is_same_v< as_const_t< int       &          >, int const &         >);
static_assert(is_same_v< as_const_t< int const &          >, int const &         >);
static_assert(is_same_v< as_const_t< int       *          >, int const *         >);
static_assert(is_same_v< as_const_t< int const *          >, int const *         >);
static_assert(is_same_v< as_const_t< int       * const    >, int const *         >);
static_assert(is_same_v< as_const_t< int const * const    >, int const *         >);
static_assert(is_same_v< as_const_t< int       *       && >, int const *         >);
static_assert(is_same_v< as_const_t< int const *       && >, int const *         >);
static_assert(is_same_v< as_const_t< int       * const && >, int const *         >);
static_assert(is_same_v< as_const_t< int const * const && >, int const *         >);
static_assert(is_same_v< as_const_t< int       *       &  >, int       * const & >);
static_assert(is_same_v< as_const_t< int const *       &  >, int const * const & >);
static_assert(is_same_v< as_const_t< int       * const &  >, int       * const & >);
static_assert(is_same_v< as_const_t< int const * const &  >, int const * const & >);
