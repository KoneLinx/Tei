
#include "Helpers.h"

using namespace tei::internal::utility;

template<typename T>
using as_const_t = decltype(as_const_projector{}(std::declval<T>()));

static_assert(std::is_same_v< as_const_t< int                  >, int const &&        >);
static_assert(std::is_same_v< as_const_t< int const            >, int const &&        >);
static_assert(std::is_same_v< as_const_t< int       &&         >, int const &&        >);
static_assert(std::is_same_v< as_const_t< int const &&         >, int const &&        >);
static_assert(std::is_same_v< as_const_t< int       &          >, int const &         >);
static_assert(std::is_same_v< as_const_t< int const &          >, int const &         >);
static_assert(std::is_same_v< as_const_t< int       *          >, int const *         >);
static_assert(std::is_same_v< as_const_t< int const *          >, int const *         >);
static_assert(std::is_same_v< as_const_t< int       * const    >, int const *         >);
static_assert(std::is_same_v< as_const_t< int const * const    >, int const *         >);
static_assert(std::is_same_v< as_const_t< int       *       && >, int const *         >);
static_assert(std::is_same_v< as_const_t< int const *       && >, int const *         >);
static_assert(std::is_same_v< as_const_t< int       * const && >, int const *         >);
static_assert(std::is_same_v< as_const_t< int const * const && >, int const *         >);
static_assert(std::is_same_v< as_const_t< int       *       &  >, int       * const & >);
static_assert(std::is_same_v< as_const_t< int const *       &  >, int const * const & >);
static_assert(std::is_same_v< as_const_t< int       * const &  >, int       * const & >);
static_assert(std::is_same_v< as_const_t< int const * const &  >, int const * const & >);
